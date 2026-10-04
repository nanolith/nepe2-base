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
