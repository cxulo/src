#! /bin/sh
#	$OpenBSD: testsuite.sh,v 1.3 2005/09/25 20:29:59 miod Exp $

# Simple test program to check what happens when userland tries to trap.
# Written by Miodrag Vallat 2003 AD -- public domain

PROG=./trap

ulimit -c 0

${PROG} 0
${PROG} 1
${PROG} 2
${PROG} 3
${PROG} 4
${PROG} 5
${PROG} 6
${PROG} 7
${PROG} 8
${PROG} 9
${PROG} 10
${PROG} 11
${PROG} 12
${PROG} 13
${PROG} 14
${PROG} 15
${PROG} 16
${PROG} 17
${PROG} 18
${PROG} 19
${PROG} 20
${PROG} 21
${PROG} 22
${PROG} 23
${PROG} 24
${PROG} 25
${PROG} 26
${PROG} 27
${PROG} 28
${PROG} 29
${PROG} 30
${PROG} 31
${PROG} 32
${PROG} 33
${PROG} 34
${PROG} 35
${PROG} 36
${PROG} 37
${PROG} 38
${PROG} 39
${PROG} 40
${PROG} 41
${PROG} 42
${PROG} 43
${PROG} 44
${PROG} 45
${PROG} 46
${PROG} 47
${PROG} 48
${PROG} 49
${PROG} 50
${PROG} 51
${PROG} 52
${PROG} 53
${PROG} 54
${PROG} 55
${PROG} 56
${PROG} 57
${PROG} 58
${PROG} 59
${PROG} 60
${PROG} 61
${PROG} 62
${PROG} 63
${PROG} 64
${PROG} 65
${PROG} 66
${PROG} 67
${PROG} 68
${PROG} 69
${PROG} 70
${PROG} 71
${PROG} 72
${PROG} 73
${PROG} 74
${PROG} 75
${PROG} 76
${PROG} 77
${PROG} 78
${PROG} 79
${PROG} 80
${PROG} 81
${PROG} 82
${PROG} 83
${PROG} 84
${PROG} 85
${PROG} 86
${PROG} 87
${PROG} 88
${PROG} 89
${PROG} 90
${PROG} 91
${PROG} 92
${PROG} 93
${PROG} 94
${PROG} 95
${PROG} 96
${PROG} 97
${PROG} 98
${PROG} 99
${PROG} 100
${PROG} 101
${PROG} 102
${PROG} 103
${PROG} 104
${PROG} 105
${PROG} 106
${PROG} 107
${PROG} 108
${PROG} 109
${PROG} 110
${PROG} 111
${PROG} 112
${PROG} 113
${PROG} 114
${PROG} 115
${PROG} 116
${PROG} 117
${PROG} 118
${PROG} 119
${PROG} 120
${PROG} 121
${PROG} 122
${PROG} 123
${PROG} 124
${PROG} 125
${PROG} 126
${PROG} 127
${PROG} 128
${PROG} 129
${PROG} 130
${PROG} 131
${PROG} 132
${PROG} 133
${PROG} 134
${PROG} 135
${PROG} 136
${PROG} 137
${PROG} 138
${PROG} 139
${PROG} 140
${PROG} 141
${PROG} 142
${PROG} 143
${PROG} 144
${PROG} 145
${PROG} 146
${PROG} 147
${PROG} 148
${PROG} 149
${PROG} 150
${PROG} 151
${PROG} 152
${PROG} 153
${PROG} 154
${PROG} 155
${PROG} 156
${PROG} 157
${PROG} 158
${PROG} 159
${PROG} 160
${PROG} 161
${PROG} 162
${PROG} 163
${PROG} 164
${PROG} 165
${PROG} 166
${PROG} 167
${PROG} 168
${PROG} 169
${PROG} 170
${PROG} 171
${PROG} 172
${PROG} 173
${PROG} 174
${PROG} 175
${PROG} 176
${PROG} 177
${PROG} 178
${PROG} 179
${PROG} 180
${PROG} 181
${PROG} 182
${PROG} 183
${PROG} 184
${PROG} 185
${PROG} 186
${PROG} 187
${PROG} 188
${PROG} 189
${PROG} 190
${PROG} 191
${PROG} 192
${PROG} 193
${PROG} 194
${PROG} 195
${PROG} 196
${PROG} 197
${PROG} 198
${PROG} 199
${PROG} 200
${PROG} 201
${PROG} 202
${PROG} 203
${PROG} 204
${PROG} 205
${PROG} 206
${PROG} 207
${PROG} 208
${PROG} 209
${PROG} 210
${PROG} 211
${PROG} 212
${PROG} 213
${PROG} 214
${PROG} 215
${PROG} 216
${PROG} 217
${PROG} 218
${PROG} 219
${PROG} 220
${PROG} 221
${PROG} 222
${PROG} 223
${PROG} 224
${PROG} 225
${PROG} 226
${PROG} 227
${PROG} 228
${PROG} 229
${PROG} 230
${PROG} 231
${PROG} 232
${PROG} 233
${PROG} 234
${PROG} 235
${PROG} 236
${PROG} 237
${PROG} 238
${PROG} 239
${PROG} 240
${PROG} 241
${PROG} 242
${PROG} 243
${PROG} 244
${PROG} 245
${PROG} 246
${PROG} 247
${PROG} 248
${PROG} 249
${PROG} 250
${PROG} 251
${PROG} 252
${PROG} 253
${PROG} 254
${PROG} 255
${PROG} 256
${PROG} 257
${PROG} 258
${PROG} 259
${PROG} 260
${PROG} 261
${PROG} 262
${PROG} 263
${PROG} 264
${PROG} 265
${PROG} 266
${PROG} 267
${PROG} 268
${PROG} 269
${PROG} 270
${PROG} 271
${PROG} 272
${PROG} 273
${PROG} 274
${PROG} 275
${PROG} 276
${PROG} 277
${PROG} 278
${PROG} 279
${PROG} 280
${PROG} 281
${PROG} 282
${PROG} 283
${PROG} 284
${PROG} 285
${PROG} 286
${PROG} 287
${PROG} 288
${PROG} 289
${PROG} 290
${PROG} 291
${PROG} 292
${PROG} 293
${PROG} 294
${PROG} 295
${PROG} 296
${PROG} 297
${PROG} 298
${PROG} 299
${PROG} 300
${PROG} 301
${PROG} 302
${PROG} 303
${PROG} 304
${PROG} 305
${PROG} 306
${PROG} 307
${PROG} 308
${PROG} 309
${PROG} 310
${PROG} 311
${PROG} 312
${PROG} 313
${PROG} 314
${PROG} 315
${PROG} 316
${PROG} 317
${PROG} 318
${PROG} 319
${PROG} 320
${PROG} 321
${PROG} 322
${PROG} 323
${PROG} 324
${PROG} 325
${PROG} 326
${PROG} 327
${PROG} 328
${PROG} 329
${PROG} 330
${PROG} 331
${PROG} 332
${PROG} 333
${PROG} 334
${PROG} 335
${PROG} 336
${PROG} 337
${PROG} 338
${PROG} 339
${PROG} 340
${PROG} 341
${PROG} 342
${PROG} 343
${PROG} 344
${PROG} 345
${PROG} 346
${PROG} 347
${PROG} 348
${PROG} 349
${PROG} 350
${PROG} 351
${PROG} 352
${PROG} 353
${PROG} 354
${PROG} 355
${PROG} 356
${PROG} 357
${PROG} 358
${PROG} 359
${PROG} 360
${PROG} 361
${PROG} 362
${PROG} 363
${PROG} 364
${PROG} 365
${PROG} 366
${PROG} 367
${PROG} 368
${PROG} 369
${PROG} 370
${PROG} 371
${PROG} 372
${PROG} 373
${PROG} 374
${PROG} 375
${PROG} 376
${PROG} 377
${PROG} 378
${PROG} 379
${PROG} 380
${PROG} 381
${PROG} 382
${PROG} 383
${PROG} 384
${PROG} 385
${PROG} 386
${PROG} 387
${PROG} 388
${PROG} 389
${PROG} 390
${PROG} 391
${PROG} 392
${PROG} 393
${PROG} 394
${PROG} 395
${PROG} 396
${PROG} 397
${PROG} 398
${PROG} 399
${PROG} 400
${PROG} 401
${PROG} 402
${PROG} 403
${PROG} 404
${PROG} 405
${PROG} 406
${PROG} 407
${PROG} 408
${PROG} 409
${PROG} 410
${PROG} 411
${PROG} 412
${PROG} 413
${PROG} 414
${PROG} 415
${PROG} 416
${PROG} 417
${PROG} 418
${PROG} 419
${PROG} 420
${PROG} 421
${PROG} 422
${PROG} 423
${PROG} 424
${PROG} 425
${PROG} 426
${PROG} 427
${PROG} 428
${PROG} 429
${PROG} 430
${PROG} 431
${PROG} 432
${PROG} 433
${PROG} 434
${PROG} 435
${PROG} 436
${PROG} 437
${PROG} 438
${PROG} 439
${PROG} 440
${PROG} 441
${PROG} 442
${PROG} 443
${PROG} 444
${PROG} 445
${PROG} 446
${PROG} 447
${PROG} 448
${PROG} 449
${PROG} 450
${PROG} 451
# since there won't be any output...
echo
${PROG} 452
${PROG} 453
${PROG} 454
${PROG} 455
${PROG} 456
${PROG} 457
${PROG} 458
${PROG} 459
${PROG} 460
${PROG} 461
${PROG} 462
${PROG} 463
${PROG} 464
${PROG} 465
${PROG} 466
${PROG} 467
${PROG} 468
${PROG} 469
${PROG} 470
${PROG} 471
${PROG} 472
${PROG} 473
${PROG} 474
${PROG} 475
${PROG} 476
${PROG} 477
${PROG} 478
${PROG} 479
${PROG} 480
${PROG} 481
${PROG} 482
${PROG} 483
${PROG} 484
${PROG} 485
${PROG} 486
${PROG} 487
${PROG} 488
${PROG} 489
${PROG} 490
${PROG} 491
${PROG} 492
${PROG} 493
${PROG} 494
${PROG} 495
${PROG} 496
${PROG} 497
${PROG} 498
${PROG} 499
${PROG} 500
${PROG} 501
${PROG} 502
${PROG} 503
${PROG} 504
${PROG} 505
${PROG} 506
${PROG} 507
${PROG} 508
${PROG} 509
${PROG} 510
${PROG} 511

exit 0
