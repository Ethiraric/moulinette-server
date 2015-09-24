# moulinette-server
This program is the server of a moulinette for EPITECH students. It will clone the git repository for a project and perform some tests on it. This should help students track and patch their errors.

## Compiling the server
Run:

    git clone https://github.com/Ethiraric/moulinette-server.git
    cd moulinette-server
    make

The executable is named 'mouliserver'.

## The config file
The configuration file is a list of key=value pairs. It must contain the following fields:
  * `clone_subfolder`: the folder in which to clone the repositories. They will be cloned to `<subfolder>/<login>/<repository>
  * `clone_login`: the login to `git clone` with (`git clone <login>@git.epitech.eu`)
  * `tests_subfolder`: the folder where all the tests are. Tests must have the following name: `<test folder>/<repository>`
  * `tests_filename`: the file which will launch the tests. It will be piped so the moulinette can get its output (stdout ONLY). It must be executable (no interpreter will be used by default)
  * `mail_sendaddr`: the mail the moulinette will use. It does not need to be a valid address (sendmail doesn't check)
  * `mail_sendername`: the name the mail will be sent with

Note that all fields are mandatory.

For security reasons, since some values may be passed directly to `system()` or to SQL functions, the characters allowed are restricted: alphanumeric characters, numbers, `-`, `_`, `.` or `@`.

Here is an example of a valid configfile:

    clone_subfolder=repositories
    clone_login=login_x
    tests_subfolder=tests
    tests_filename=run_tests.py
    mail_sendaddr=mail@example.com
    mail_sendername=mouli

## Setting up your test server
You should call mouliserver the following way:

    ./mouliserver <port> <sqlitefile> <configfile>

The sqlite database must have been created beforehand (using `create.sql`) with sqlite3 (`cat create.sql | sqlite3 <sqlitefile>` will do the trick).

## Bugs and Suggestions
Feel free to report anything you think useful to florian.sabourin@epitech.eu.
