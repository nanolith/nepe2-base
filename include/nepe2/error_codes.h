/**
 * \file nepe2/error_codes.h
 *
 * \brief Error codes for nepe library.
 *
 * \copyright 2023 Justin Handville.  Please see License.txt in this
 * distribution for the license terms under which this software is distributed.
 */

#pragma once

#define ERROR_METADATA_FIELD_NOT_SET                                    0x3401
#define ERROR_METADATA_BAD_ENCODING_LENGTH                              0x3402
#define ERROR_METADATA_INVALID_BUFFER_SIZE                              0x3403
#define ERROR_METADATA_UNKNOWN_SERIAL_VERSION                           0x3404
#define ERROR_TRANSFORMER_REGISTRY_CANNOT_BE_RELEASED                   0x3405
#define ERROR_TRANSFORMER_REGISTRY_NAME_ALREADY_REGISTERED              0x3406
#define ERROR_TRANSFORMER_REGISTRY_NOT_FOUND                            0x3407
#define ERROR_MAPPER_REGISTRY_CANNOT_BE_RELEASED                        0x3408
#define ERROR_MAPPER_REGISTRY_NAME_ALREADY_REGISTERED                   0x3409
#define ERROR_MAPPER_REGISTRY_NOT_FOUND                                 0x340a
#define ERROR_DATABASE_MDB_ENV_CREATE                                   0x340b
#define ERROR_DATABASE_MDB_ENV_SET_MAPSIZE                              0x340c
#define ERROR_DATABASE_MDB_ENV_SET_MAXDBS                               0x340d
#define ERROR_DATABASE_MDB_ENV_OPEN                                     0x340e
#define ERROR_DATABASE_MDB_TXN_BEGIN                                    0x340f
#define ERROR_DATABASE_MDB_DBI_OPEN                                     0x3410
#define ERROR_DATABASE_MDB_TXN_COMMIT                                   0x3411
#define ERROR_DATABASE_MAP_FULL                                         0x3412
#define ERROR_DATABASE_MAP_RESIZE                                       0x3413
#define ERROR_DATABASE_MAP_RESIZE_FAILURE                               0x3414
#define ERROR_DATABASE_ENVINFO                                          0x3415
#define ERROR_DATABASE_SIZE_NOT_FOUND                                   0x3416
#define ERROR_DATABASE_SIZE_QUERY_FAILED                                0x3417
#define ERROR_DATABASE_SIZE_BAD                                         0x3418
#define ERROR_DATABASE_SIZE_UPDATE                                      0x3419
#define ERROR_DATABASE_SCHEMA_NOT_FOUND                                 0x341a
#define ERROR_DATABASE_SCHEMA_QUERY_FAILED                              0x341b
#define ERROR_DATABASE_SCHEMA_BAD                                       0x341c
#define ERROR_DATABASE_SCHEMA_UPDATE                                    0x341d
#define ERROR_DATABASE_SCHEMA_NEEDS_UPGRADE                             0x341e
#define ERROR_DATABASE_SCHEMA_UNSUPPORTED                               0x341f
#define ERROR_DATABASE_SALT_NOT_FOUND                                   0x3420
#define ERROR_DATABASE_SALT_QUERY_FAILED                                0x3421
#define ERROR_DATABASE_SALT_GENERATE_FAILED                             0x3422
#define ERROR_DATABASE_SALT_UPDATE                                      0x3423
#define ERROR_DATABASE_ENCRYPTION_KEY_CREATE                            0x3424
#define ERROR_TERMINAL_READPASSPHRASE                                   0x3425
#define ERROR_TERMINAL_STDIN_NOT_A_TERMINAL                             0x3426
#define ERROR_TERMINAL_TCSETATTR                                        0x3427
#define ERROR_TERMINAL_BAD_CHOICE                                       0x3428
#define ERROR_DATABASE_VALUE_IV_GENERATE_FAILED                         0x3429
#define ERROR_DATABASE_DERIVED_KEY_CREATE                               0x342a
#define ERROR_DATABASE_CIPHER_CREATE                                    0x342b
#define ERROR_DATABASE_VALUE_MAC                                        0x342c
#define ERROR_DATABASE_METADATA_UPSERT_FAILURE                          0x342d
#define ERROR_DATABASE_WOULD_OVERWRITE                                  0x342e
#define ERROR_SECURE_BUFFER_BASE64_DECODE                               0x342f
#define ERROR_DATABASE_VALUE_BAD_BLOB                                   0x3430
