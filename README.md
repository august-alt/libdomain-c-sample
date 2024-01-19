# Documentation for the LDAP Search Example Program using the libdomain Library

## Overview

The provided example is a C program that demonstrates the use of the libdomain library to perform an LDAP search operation. The program establishes a connection to the LDAP server and executes a search in the specified directory server.

## Compilation

To compile the program, you need to install the libdomain library.

```bash
apt-get install git cmake make gcc krb5-kinit libdomain-devel libconfig-devel
```

Clone the example:

```bash
git clone https://github.com/libdomain/libdomain-c-sample
```

Use the following compilation command:

```bash
cd libdomain-c-sample && mkdir build && cd build && cmake .. && make -j `nproc`
```

## Usage
The program takes command-line arguments to specify the connection parameters to the LDAP server and the search parameters. For this example, we assume that we have deployed a testbed based on Samba DC called `dc` in the domain `example.org`, in which the LDAP service is running on port 389.

Make sure that you use the correct connection data, such as username and password, etc. If the domain name or LDAP service is different, you may need to change the parameters passed to the example in accordance with the settings of the LDAP service.

Samba and Windows Active Directory can support Kerberos authentication depending on the configuration. If the server supports GSSAPI and Kerberos, make sure that you have passed the "--sasl" parameter. In this case, you may also need to generate a Kerberos ticket to do this, you may need to run the kinit command.

```bash
kinit administrator@example.org
```
To run sample execute following command from `build` directory:

```bash
./libdomain-c-sample --host ldap://dc.example.org:389 --user administrator --pass password --bind "dc=example,dc=org" --sasl
```

### Command Line Options

- `--host (-h)`: Specifies the LDAP server in the format "protocol://address:port," for example, "ldap://example.org:389."
- `--user (-u)`: Specifies the LDAP username.
- `--pass (-w)`: Specifies the LDAP password.
- `--bind (-b)`: Specifies the Distinguished Name (DN) for LDAP binding, for example, "dc=example,dc=org."
- `--sasl (-s)`: Enables the use of SASL binding.

## Program Structure

1. **Event Handling Functions:**
   - `exit_callback`: Triggers the program exit event.
   - `connection_on_error`: Handles errors during LDAP operations.
   - `connection_on_update`: Handles connection state updates and errors during connection establishment.

2. **Option Parsing:**
   - `parse_opt`: Parses command-line options using the argp library.

3. **Main Function:**
   - First, a new talloc context is created using the `talloc_new` function.
   - The `arguments` structure is initialized to store command-line arguments.
   - Command-line arguments are processed using the `argp_parse` function.
   - Mandatory arguments, such as host and bind_dn, are checked, and the program exits if these arguments are not found.
   - A configuration structure for connecting to the LDAP server is created using the `ld_config` function.
   - The main library handle is initialized using the `ld_init` function.
   - Default event handlers are installed using `ld_install_default_handlers(handle)`.
   - An event handler with the main program logic is set using `ld_install_handler(handle, connection_on_update, update_interval)`.
   - An exit handler that shuts down the program after 10 seconds is installed using `ld_install_handler(handle, exit_callback, exit_time)`.
   - Error handlers are set.
   - The main event loop is started with `ld_exec`.
   - LDAP search is performed.
   - Resources are cleaned up using `ld_free(handle)` and `talloc_free(talloc_ctx)` functions.

## Error Handling

Errors, such as the inability to perform LDAP operations, lead to program termination with the appropriate error message. Error handling is implemented in the `connection_on_error` function. However, connection error handling occurs in the `connection_on_update` function.

## Notes

- The program performs an LDAP search when the connection state changes to `LDAP_CONNECTION_STATE_RUN`.
- Objects for search are specified in the `LDAP_DIRECTORY_ATTRS` variable.

## Additional Information

- [GNU Argp Example](https://www.gnu.org/software/libc/manual/html_node/Argp-Example-3.html)

## Version Information

- Program Version: 1.0.0

## License

This program is distributed under the GPLv2 license. See the accompanying LICENSE file for detailed information.
