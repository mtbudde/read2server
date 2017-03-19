#include "libssh2_config.h"
#include <libssh2.h>
#include <libssh2_sftp.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#include "read2serverfunc.h"

int main(int argc, char *argv[])
{
  LIBSSH2_SFTP *sftp_session = initializeCon( "10.42.0.198", "root", "research418" );
  int writeStatus = writeFile( sftp_session, "test.txt", "/root/test.txt" );
//
//     unsigned long hostaddr = inet_addr( "10.42.0.198" );
//     int sock, i, auth_pw = 1;
//     struct sockaddr_in sin;
//     const char *fingerprint;
//     LIBSSH2_SESSION *session;
//     const char *username="root";
//     const char *password="research418";
//     const char *loclfile="test.txt";
//     const char *sftppath="/root/test.txt";
//     int rc;
//     FILE *local;
//     LIBSSH2_SFTP *sftp_session;
//     LIBSSH2_SFTP_HANDLE *sftp_handle;
//     char mem[1024*100];
//     size_t nread;
//     char *ptr;
//
//     rc = libssh2_init (0);
//     if (rc != 0) {
//         fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
//         return 1;
//     }
//
//     local = fopen(loclfile, "rb");
//     if (!local) {
//         fprintf(stderr, "Can't open local file %s\n", loclfile);
//         return -1;
//     }
//
//     /*
//      * The application code is responsible for creating the socket
//      * and establishing the connection
//      */
//     sock = socket(AF_INET, SOCK_STREAM, 0);
//
//     sin.sin_family = AF_INET;
//     sin.sin_port = htons(22);
//     sin.sin_addr.s_addr = hostaddr;
//     if (connect(sock, (struct sockaddr*)(&sin),
//             sizeof(struct sockaddr_in)) != 0) {
//         fprintf(stderr, "failed to connect!\n");
//         return -1;
//     }
//
//     /* Create a session instance
//      */
//     session = libssh2_session_init();
//     if(!session)
//         return -1;
//
//     /* Since we have set non-blocking, tell libssh2 we are blocking */
//     libssh2_session_set_blocking(session, 1);
//
//     /* ... start it up. This will trade welcome banners, exchange keys,
//      * and setup crypto, compression, and MAC layers
//      */
//     rc = libssh2_session_handshake(session, sock);
//     if(rc) {
//         fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
//         return -1;
//     }
//
//     /* At this point we havn't yet authenticated.  The first thing to do
//      * is check the hostkey's fingerprint against our known hosts Your app
//      * may have it hard coded, may go to a file, may present it to the
//      * user, that's your call
//      */
//     fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
//     fprintf(stderr, "Fingerprint: ");
//     for(i = 0; i < 20; i++) {
//         fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
//     }
//     fprintf(stderr, "\n");
//
//     if (auth_pw) {
//         /* We could authenticate via password */
//         if (libssh2_userauth_password(session, username, password)) {
//             fprintf(stderr, "Authentication by password failed.\n");
//             goto shutdown;
//         }
//     } else {
//         /* Or by public key */
//         if (libssh2_userauth_publickey_fromfile(session, username,
//                             "/home/username/.ssh/id_rsa.pub",
//                             "/home/username/.ssh/id_rsa",
//                             password)) {
//             fprintf(stderr, "\tAuthentication by public key failed\n");
//             goto shutdown;
//         }
//     }
//
//     fprintf(stderr, "libssh2_sftp_init()!\n");
//     sftp_session = libssh2_sftp_init(session);
//
//     if (!sftp_session) {
//         fprintf(stderr, "Unable to init SFTP session\n");
//         goto shutdown;
//     }
//
//     fprintf(stderr, "libssh2_sftp_open()!\n");
//     /* Request a file via SFTP */
//     sftp_handle =
//         libssh2_sftp_open(sftp_session, sftppath,
//                       LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
//                       LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
//                       LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);
//
//     if (!sftp_handle) {
//         fprintf(stderr, "Unable to open file with SFTP\n");
//         goto shutdown;
//     }
//     fprintf(stderr, "libssh2_sftp_open() is done, now send data!\n");
//     do {
//         nread = fread(mem, 1, sizeof(mem), local);
//         if (nread <= 0) {
//             /* end of file */
//             break;
//         }
//         ptr = mem;
//
//         do {
//             /* write data in a loop until we block */
//             rc = libssh2_sftp_write(sftp_handle, ptr, nread);
//             if(rc < 0)
//                 break;
//             ptr += rc;
//             nread -= rc;
//         } while (nread);
//
//     } while (rc > 0);
//
//     libssh2_sftp_close(sftp_handle);
//     libssh2_sftp_shutdown(sftp_session);
//
// shutdown:
//     libssh2_session_disconnect(session,
//                                "Normal Shutdown, Thank you for playing");
//     libssh2_session_free(session);
//
//     close(sock);
//
//     if (local)
//         fclose(local);
//     fprintf(stderr, "all done\n");
//
//     libssh2_exit();

    return 0;
}
