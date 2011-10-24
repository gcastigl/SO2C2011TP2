#ifndef PERMISSION_H
#define	PERMISSION_H

#include <defs.h>
#include <access/user.h>

#include <util/logger.h>

#define R_BIT           0000004	/* Rwx protection bit */
#define W_BIT           0000002	/* rWx protection bit */
#define X_BIT           0000001	/* rwX protection bit */

#define S_IRWXU 00700 //User RWX
#define S_IRUSR 00400 //User R
#define S_IWUSR 00200 //User W
#define S_IXUSR 00100 //User X
#define S_IRWXG 00070 //Group RWX
#define S_IRGRP 00040 //Group R
#define S_IWGRP 00020 //Group W
#define S_IXGRP 00010 //Group X
#define S_IRWXO 00007 //Others RWX
#define S_IROTH 00004 //Others R
#define S_IWOTH 00002 //Others W
#define S_IXOTH 00001 //Others X


#endif	/* PERMISSION_H */

