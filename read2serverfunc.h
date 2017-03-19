//#include "read2serverfunc.c"

LIBSSH2_SFTP * initializeCon( const char *ip, const char *username, const char *password );

int writeFile( LIBSSH2_SFTP *sftp_session, const char *loclfile, const char *sftppath );

int closeCon();
