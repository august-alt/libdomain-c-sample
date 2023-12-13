#include <argp.h>
#include <libdomain/domain.h>
#include <libdomain/directory.h>
#include <libdomain/entry.h>
#include <libdomain/connection_state_machine.h>
#include <stdio.h>
#include <stdbool.h>
#include <talloc.h>

static char* LDAP_DIRECTORY_ATTRS[] = { "objectClass", NULL };

static void exit_callback(verto_ctx *ctx, verto_ev *ev)
{
    (void) ctx;
    (void) ev;

    verto_break(ctx);
}

static enum OperationReturnCode connection_on_error(int rc, void* unused_a, void* connection)
{
    (void)(unused_a);

    verto_break(((ldap_connection_ctx_t*)connection)->base);

    error("Unable to perform operation!\n");

    exit(EXIT_FAILURE);

    return RETURN_CODE_SUCCESS;
}

static void connection_on_update(verto_ctx *ctx, verto_ev *ev)
{
    (void)(ctx);

    struct ldap_connection_ctx_t* connection = verto_get_private(ev);

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_RUN)
    {
        verto_del(ev);

        search(connection, "dc=domain,dc=alt", LDAP_SCOPE_SUBTREE,
               "(objectClass=*)", LDAP_DIRECTORY_ATTRS, 0, NULL);
    }

    if (connection->state_machine->state == LDAP_CONNECTION_STATE_ERROR)
    {
        verto_break(ctx);

        error("Error encountered during bind!\n");
    }
}

const char *argp_program_version = "1.0.0";
const char *argp_program_bug_address = "https://bugzilla.altlinux.org";

static char doc[] = "Libdomain get RootDSE sample.";
static char args_doc[] = "[HOSTNAME] [BINDDN] <USERNAME> <PASSWORD> <USE_SASL>";

static struct argp_option options[] =
{
    { "host", 'h', "HOST", 0, "Host. Use protocol://adress:port format e.g. ldap://dc.example.org:389."},
    { "user", 'u', "USER", 0, "User name."},
    { "pass", 'w', "PASS", 0, "Password."},
    { "sasl", 's', "SASL", 0, "Whether or not use SASL bind."},
    { "bind", 'b', "BIND", 0, "Bind dn. For example: \"dc=example,dc=org\"."},
    { 0 }
};

struct arguments
{
    char* hostname;
    char* password;
    char* username;
    char* bind_dn;
    bool useSasl;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key)
    {
    case 'h':
        arguments->hostname = arg;
        break;
    case 'u':
        arguments->username = arg;
        break;
    case 'w':
        arguments->password = arg;
        break;
    case 's':
        arguments->useSasl = true;
        break;
    case 'b':
        arguments->bind_dn = arg;
        break;
    case ARGP_KEY_ARG:
        return 0;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, NULL, NULL, NULL };

int main(int argc, char **argv)
{
    TALLOC_CTX* talloc_ctx = talloc_new(NULL);

    struct arguments arguments;

    arguments.hostname = NULL;
    arguments.username = NULL;
    arguments.password = NULL;
    arguments.bind_dn = NULL;
    arguments.useSasl = false;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (arguments.hostname == NULL)
    {
        printf("Error: Missing required argument --host\n");
        exit(EXIT_FAILURE);
    }

    if (arguments.bind_dn == NULL)
    {
        printf("Error: Missing required argument --bind\n");
        exit(EXIT_FAILURE);
    }

    const int update_interval = 1000;

    ld_config_t *config = NULL;
    config = ld_create_config(talloc_ctx, arguments.hostname, 0, LDAP_VERSION3, "dc=domain,dc=alt",
                              arguments.username, arguments.password, !arguments.useSasl, false, arguments.useSasl, false,
                              update_interval, "", "", "");

    const int exit_time = 10000;

    LDHandle *handle = NULL;
    ld_init(&handle, config);

    ld_install_default_handlers(handle);
    ld_install_handler(handle, connection_on_update, update_interval);
    ld_install_handler(handle, exit_callback, exit_time);
    ld_install_error_handler(handle, connection_on_error);

    ld_exec(handle);

    ld_free(handle);

    talloc_free(talloc_ctx);

    return 0;
}
