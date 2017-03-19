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

// A function to set up the connection to the SFTP server at the specified address
LIBSSH2_SFTP *initializeCon( const char *ip, const char *username, const char *password ) {
  unsigned long hostaddr = inet_addr( ip );
  int sock, i, auth_pw = 1;
  struct sockaddr_in sin;
  const char *fingerprint;
  int rc;
  LIBSSH2_SESSION *session;
  LIBSSH2_SFTP *sftp_session;

  rc = libssh2_init (0);
       if (rc != 0) {
           fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
           return 1;
       }

  sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = hostaddr;
    if (connect(sock, (struct sockaddr*)(&sin),
            sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!\n");
        return -1;
    }

    /* Create a session instance
     */
    session = libssh2_session_init();
    if(!session)
        return -1;

    /* Since we have set non-blocking, tell libssh2 we are blocking */
    libssh2_session_set_blocking(session, 1);

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    rc = libssh2_session_handshake(session, sock);
    if(rc) {
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
        return -1;
    }

    /* At this point we havn't yet authenticated.  The first thing to do
     * is check the hostkey's fingerprint against our known hosts Your app
     * may have it hard coded, may go to a file, may present it to the
     * user, that's your call
     */
    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    fprintf(stderr, "Fingerprint: ");
    for(i = 0; i < 20; i++) {
        fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
    }
    fprintf(stderr, "\n");

    if (auth_pw) {
        /* We could authenticate via password */
        if (libssh2_userauth_password(session, username, password)) {
            fprintf(stderr, "Authentication by password failed.\n");
            libssh2_session_disconnect(session,
                                          "Normal Shutdown, Thank you for playing");
               libssh2_session_free(session);

               close(sock);

               fprintf(stderr, "all done\n");

               libssh2_exit();
        }
    } else {
        /* Or by public key */
        if (libssh2_userauth_publickey_fromfile(session, username,
                            "/home/username/.ssh/id_rsa.pub",
                            "/home/username/.ssh/id_rsa",
                            password)) {
            fprintf(stderr, "\tAuthentication by public key failed\n");
            libssh2_session_disconnect(session,
                                          "Normal Shutdown, Thank you for playing");
               libssh2_session_free(session);

               close(sock);

               fprintf(stderr, "all done\n");

               libssh2_exit();
        }
    }

    fprintf(stderr, "libssh2_sftp_init()!\n");
    sftp_session = libssh2_sftp_init(session);

    if (!sftp_session) {
        fprintf(stderr, "Unable to init SFTP session\n");
        libssh2_session_disconnect(session,
                                      "Normal Shutdown, Thank you for playing");
           libssh2_session_free(session);

           close(sock);

           fprintf(stderr, "all done\n");

           libssh2_exit();
    }

    fprintf(stderr, "libssh2_sftp_open()!\n");
}

int writeFile( LIBSSH2_SFTP *sftp_session, const char *loclfile, const char *sftppath ) {
  LIBSSH2_SFTP_HANDLE *sftp_handle;
  size_t nread;
  char mem[1024*100];
  FILE *local;
  int rc;
  char *ptr;

  local = fopen(loclfile, "rb");
      if (!local) {
          fprintf(stderr, "Can't open local file %s\n", loclfile);
          return -1;
      }

  sftp_handle =
        libssh2_sftp_open(sftp_session, sftppath,
                      LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
                      LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
                      LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);

    // if (!sftp_handle) {
    //     fprintf(stderr, "Unable to open file with SFTP\n");
    //     libssh2_session_disconnect(session,
    //     //                                "Normal Shutdown, Thank you for playing");
    //     //     libssh2_session_free(session);
    //     //
    //     //     close(sock);
    //     //
    //     //     if (local)
    //     //         fclose(local);
    //     //     fprintf(stderr, "all done\n");
    //     //
    //     //     libssh2_exit();
    // }
    fprintf(stderr, "libssh2_sftp_open() is done, now send data!\n");
    do {
        nread = fread(mem, 1, sizeof(mem), local);
        if (nread <= 0) {
            /* end of file */
            break;
        }
        ptr = mem;

        do {
            /* write data in a loop until we block */
            rc = libssh2_sftp_write(sftp_handle, ptr, nread);
            if(rc < 0)
                break;
            ptr += rc;
            nread -= rc;
        } while (nread);

    } while (rc > 0);

    libssh2_sftp_close(sftp_handle);
    libssh2_sftp_shutdown(sftp_session);
}

/*
int closeCon( LIBSSH2_SESSION *session ) {
  libssh2_session_disconnect(session,
                               "Normal Shutdown, Thank you for playing");
    libssh2_session_free(session);

    close(sock);

    if (local)
        fclose(local);
    fprintf(stderr, "all done\n");

    libssh2_exit();

    return 0;
}
*/
