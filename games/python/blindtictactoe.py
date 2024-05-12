# dumping propnet for /Users/zac/work/comp/thesis/code/rulesheets_gdl2/blindtictactoe.gdl
# number of roles 3

from constants import *

roles = [
    'x',
    'o',
    'random',
]

entries = (
    (0, -1, PROPOSITION, [241], [452, 144, 354, 160, 85, 87, 413, 77, 10, 526, 52, 567, 480, 113, 430, 96, 198, 98], base, '( true ( cell 1 2 b ) )'),
    (1, -1, PROPOSITION, [460], [420, 305, 535, 145, 121, 480], other, '( next_tmp5 3 )'),
    (2, -1, PROPOSITION, [542], [154], other, '( marked 3 2 )'),
    (3, -1, PROPOSITION, [11], [220, 523], other, '( sees_tmp3 1 o )'),
    (4, -1, AND, [57, 301], [442]),
    (5, -1, AND, [426, 192, 236], [437]),
    (6, -1, OR, [189, 171], [39]),
    (7, -1, PROPOSITION, [36], [505, 282, 120, 421, 126, 206, 475, 168, 8, 305, 413, 415, 197, 312, 136, 175, 313, 218], base, '( true ( cell 1 3 b ) )'),
    (8, -1, AND, [7, 260, 528], [45]),
    (9, -1, OR, [150, 171], [146]),
    (10, -1, AND, [0, 268], [442]),
    (11, -1, OR, [40, 551, 18], [3]),
    (12, -1, AND, [244, 196], [102]),
    (13, -1, OR, [41, 123], [330]),
    (14, -1, PROPOSITION, [30], [224, 226, 516, 205, 167, 413, 310, 210, 495, 540, 278, 371, 446, 543, 499, 156, 115, 240], base, '( true ( cell 3 3 b ) )'),
    (15, -1, NOT, [530], [249]),
    (16, -1, PROPOSITION, [187], [67, 208], base, '( true ( tried x 1 2 ) )'),
    (17, -1, AND, [406, 229], [102]),
    (18, -1, AND, [41, 455], [11]),
    (19, -1, OR, [500, 82, 153, 528, 57, 268], [196]),
    (20, -1, TRANSITION, [560], [372]),
    (21, -1, PROPOSITION, [461], [321], other, '( column 1 o )'),
    (22, -1, PROPOSITION, [89], [507, 501], base, '( true ( tried o 2 1 ) )'),
    (23, -1, PROPOSITION, [321], [502, 478, 263, 358, 388, 336], other, '( line o )'),
    (24, -1, AND, [502, 459], [106]),
    (25, -1, AND, [236, 73], [333]),
    (26, -1, AND, [317, 84], [189]),
    (27, -1, OR, [62, 228], [487]),
    (28, -1, OR, [220, 200, 142, 552, 553, 127, 489, 564, 523, 133, 256, 376, 242], [29]),
    (29, -1, PROPOSITION, [28], [], sees, '( sees o ok )'),
    (30, -1, TRANSITION, [265], [14]),
    (31, -1, OR, [140, 526, 344], [472]),
    (32, -1, NOT, [111], [120]),
    (33, -1, OR, [82, 268], [428]),
    (34, -1, PROPOSITION, [295], [549, 193, 122], base, '( true ( cell 1 2 x ) )'),
    (35, -1, PROPOSITION, [549], [78], other, '( row 1 x )'),
    (36, -1, TRANSITION, [72], [7]),
    (37, -1, OR, [153, 71], [339]),
    (38, -1, AND, [131, 196], [102]),
    (39, -1, TRANSITION, [6], [84]),
    (40, -1, AND, [57, 301], [11]),
    (41, -1, PROPOSITION, [], [55, 391, 227, 535, 356, 422, 164, 47, 75, 507, 364, 13, 18, 68, 467], input, '( does o ( mark 2 1 ) )'),
    (42, -1, AND, [185, 301], [130]),
    (43, -1, PROPOSITION, [349], [], legal, '( legal random ( tiebreak x ) )'),
    (44, -1, AND, [425, 316, 173], [202]),
    (45, -1, OR, [168, 8, 415, 197, 312, 362], [212]),
    (46, -1, AND, [316, 525, 173, 531], [351]),
    (47, -1, AND, [41, 455], [162]),
    (48, -1, PROPOSITION, [437], [479], other, '( next ( cell 3 1 o ) )'),
    (49, -1, PROPOSITION, [449], [445, 410, 458, 347], base, '( true ( cell 1 3 o ) )'),
    (50, -1, PROPOSITION, [490], [445, 360, 186], base, '( true ( cell 2 3 o ) )'),
    (51, -1, PROPOSITION, [193], [295], other, '( next ( cell 1 2 x ) )'),
    (52, -1, AND, [82, 525, 0, 268], [414]),
    (53, -1, PROPOSITION, [327], [317], other, '( marked 2 3 )'),
    (54, -1, OR, [251, 565, 254, 454, 372, 456], [560]),
    (55, -1, AND, [426, 41, 455], [473]),
    (56, -1, TRANSITION, [547], [545]),
    (57, -1, PROPOSITION, [], [180, 40, 145, 164, 323, 361, 4, 559, 290, 364, 19, 373, 431, 448, 139], input, '( does o ( mark 1 1 ) )'),
    (58, -1, AND, [84, 137], [442]),
    (59, -1, AND, [84, 137, 105], [186]),
    (60, -1, PROPOSITION, [458], [321], other, '( row 1 o )'),
    (61, -1, PROPOSITION, [80], [493], other, '( next ( cell 3 3 x ) )'),
    (62, -1, PROPOSITION, [], [450, 420, 435, 319, 183, 101, 27, 422, 288, 75, 128, 273, 344, 176, 258], input, '( does o ( mark 2 2 ) )'),
    (63, -1, AND, [426, 84, 137], [186]),
    (64, -1, PROPOSITION, [163], [538, 122, 255, 147, 375], base, '( true ( cell 2 2 x ) )'),
    (65, -1, PROPOSITION, [169], [303], other, '( next ( tried o 3 1 ) )'),
    (66, -1, AND, [324, 93, 238], [375]),
    (67, -1, OR, [82, 16], [314]),
    (68, -1, AND, [41, 525, 455, 123], [473]),
    (69, -1, AND, [324, 425, 238], [375]),
    (70, -1, PROPOSITION, [83], [300], other, '( next ( cell 2 2 o ) )'),
    (71, -1, PROPOSITION, [110], [494, 37], base, '( true ( tried x 1 1 ) )'),
    (72, -1, OR, [171, 219], [36]),
    (73, -1, PROPOSITION, [], [25, 512, 454, 104, 164, 456, 304, 88, 460, 271, 363, 251, 565, 254, 544], input, '( does x ( mark 3 1 ) )'),
    (74, -1, OR, [515, 171], [152]),
    (75, -1, OR, [62, 41, 137], [315]),
    (76, -1, PROPOSITION, [102], [], sees, '( sees x ok )'),
    (77, -1, AND, [82, 315, 0], [193]),
    (78, -1, OR, [329, 119, 35, 370, 398, 488, 546], [459]),
    (79, -1, TRANSITION, [291], [250]),
    (80, -1, OR, [226, 278, 516, 499, 296, 205], [61]),
    (81, -1, PROPOSITION, [239], [477, 396], base, '( true ( tried x 2 2 ) )'),
    (82, -1, PROPOSITION, [], [144, 160, 85, 101, 87, 77, 274, 33, 67, 52, 19, 113, 430, 198, 399], input, '( does x ( mark 1 2 ) )'),
    (83, -1, OR, [450, 420, 183, 288, 258, 166], [70]),
    (84, -1, PROPOSITION, [39], [401, 402, 405, 245, 262, 58, 26, 121, 59, 476, 63, 413, 275, 293, 496, 389, 177, 280], base, '( true ( cell 2 3 b ) )'),
    (85, -1, AND, [82, 0], [259]),
    (86, -1, PROPOSITION, [477], [239], other, '( next ( tried x 2 2 ) )'),
    (87, -1, AND, [82, 0, 93], [193]),
    (88, -1, AND, [236, 73], [252]),
    (89, -1, TRANSITION, [379], [22]),
    (90, -1, TRANSITION, [487], [228]),
    (91, -1, PROPOSITION, [217], [202, 367, 122], base, '( true ( cell 3 2 x ) )'),
    (92, -1, PROPOSITION, [537], [519, 443], base, '( true ( tried x 2 3 ) )'),
    (93, -1, PROPOSITION, [204], [565, 465, 66, 417, 87, 97], other, '( next_tmp3 3 )'),
    (94, -1, AND, [315, 316, 173], [202]),
    (95, -1, PROPOSITION, [478], [], terminal, 'terminal'),
    (96, -1, AND, [403, 0], [557]),
    (97, -1, AND, [153, 93, 301], [436]),
    (98, -1, AND, [404, 0, 268], [414]),
    (99, -1, PROPOSITION, [186], [490], other, '( next ( cell 2 3 o ) )'),
    (100, -1, PROPOSITION, [108], [286, 486], other, '( sees_tmp3 3 x )'),
    (101, -1, OR, [324, 82, 62, 173, 268, 531], [451]),
    (102, -1, OR, [182, 286, 486, 555, 337, 12, 17, 366, 369, 498, 257, 216, 38], [76]),
    (103, -1, NOT, [340], [573]),
    (104, -1, OR, [153, 123, 73], [233]),
    (105, -1, PROPOSITION, [274], [227, 310, 183, 59, 302, 165], other, '( next_tmp5 1 )'),
    (106, -1, PROPOSITION, [24], [], goal, '( goal x 100 )'),
    (107, -1, AND, [455, 123], [397]),
    (108, -1, OR, [136, 446, 280], [100]),
    (109, -1, PROPOSITION, [514], [462, 536, 357, 445], base, '( true ( cell 3 3 o ) )'),
    (110, -1, TRANSITION, [339], [71]),
    (111, -1, PROPOSITION, [135], [32], other, '( marked 1 3 )'),
    (112, -1, PROPOSITION, [360], [321], other, '( row 2 o )'),
    (113, -1, AND, [82, 0, 268, 518], [193]),
    (114, -1, AND, [482, 123, 455], [522]),
    (115, -1, AND, [14, 207], [383]),
    (116, -1, TRANSITION, [381], [497]),
    (117, -1, OR, [171, 557], [241]),
    (118, -1, PROPOSITION, [479], [462, 437, 410, 461], base, '( true ( cell 3 1 o ) )'),
    (119, -1, PROPOSITION, [444], [78], other, '( column 3 x )'),
    (120, -1, AND, [7, 32], [219]),
    (121, -1, AND, [84, 1, 137], [186]),
    (122, -1, AND, [64, 34, 91], [488]),
    (123, -1, PROPOSITION, [], [503, 161, 267, 422, 104, 164, 289, 107, 231, 365, 13, 465, 68, 114, 467], input, '( does x ( mark 2 1 ) )'),
    (124, -1, PROPOSITION, [276], [504, 457], base, '( true ( tried x 1 3 ) )'),
    (125, -1, AND, [153, 301], [397]),
    (126, -1, AND, [7, 500], [190]),
    (127, -1, AND, [380, 229], [28]),
    (128, -1, AND, [62, 238], [162]),
    (129, -1, PROPOSITION, [164], [133, 553, 486, 216], other, '( sees_tmp9 1 )'),
    (130, -1, PROPOSITION, [42], [411], other, '( next ( cell 1 1 b ) )'),
    (131, -1, PROPOSITION, [252], [337, 38], other, '( sees_tmp1 3 x )'),
    (132, -1, AND, [316, 404, 531], [351]),
    (133, -1, AND, [129, 472], [28]),
    (134, -1, AND, [520, 502, 298], [230]),
    (135, -1, OR, [500, 528], [111]),
    (136, -1, AND, [7, 528], [108]),
    (137, -1, PROPOSITION, [], [402, 331, 405, 262, 58, 121, 422, 204, 59, 75, 63, 275, 385, 293, 327], input, '( does o ( mark 2 3 ) )'),
    (138, -1, PROPOSITION, [263], [], goal, '( goal o 100 )'),
    (139, -1, AND, [153, 525, 57, 301], [562]),
    (140, -1, AND, [316, 531], [31]),
    (141, -1, PROPOSITION, [436], [491], other, '( next ( cell 1 1 x ) )'),
    (142, -1, AND, [380, 572], [28]),
    (143, -1, PROPOSITION, [285], [510, 214], base, '( true ( tried o 3 3 ) )'),
    (144, -1, AND, [82, 0], [333]),
    (145, -1, AND, [57, 1, 301], [562]),
    (146, -1, TRANSITION, [9], [455]),
    (147, -1, AND, [64, 372, 362], [325]),
    (148, -1, TRANSITION, [212], [362]),
    (149, -1, PROPOSITION, [418], [], legal, '( legal x ( mark 3 3 ) )'),
    (150, -1, PROPOSITION, [439], [9], other, '( next ( cell 2 1 b ) )'),
    (151, -1, TRANSITION, [539], [199]),
    (152, -1, TRANSITION, [74], [236]),
    (153, -1, PROPOSITION, [], [328, 246, 355, 104, 164, 125, 361, 274, 326, 309, 19, 431, 97, 37, 139], input, '( does x ( mark 1 1 ) )'),
    (154, -1, NOT, [2], [232]),
    (155, -1, TRANSITION, [348], [382]),
    (156, -1, AND, [14, 471], [252]),
    (157, -1, PROPOSITION, [281], [292, 418], base, '( true ( tried x 3 3 ) )'),
    (158, -1, PROPOSITION, [491], [549, 538, 384, 436], base, '( true ( cell 1 1 x ) )'),
    (159, -1, PROPOSITION, [508], [], legal, '( legal x ( mark 3 2 ) )'),
    (160, -1, AND, [82, 0], [297]),
    (161, -1, AND, [455, 123], [333]),
    (162, -1, OR, [405, 47, 128], [571]),
    (163, -1, TRANSITION, [172], [64]),
    (164, -1, OR, [153, 41, 57, 192, 123, 73], [129]),
    (165, -1, AND, [316, 531, 105], [351]),
    (166, -1, PROPOSITION, [300], [83, 536, 360, 410, 338], base, '( true ( cell 2 2 o ) )'),
    (167, -1, AND, [426, 14, 207], [357]),
    (168, -1, AND, [7, 315, 528], [45]),
    (169, -1, OR, [192, 213], [65]),
    (170, -1, PROPOSITION, [351], [221], other, '( next ( cell 3 2 o ) )'),
    (171, -1, PROPOSITION, [], [378, 6, 117, 411, 9, 72, 265, 368, 74], init, 'init'),
    (172, -1, PROPOSITION, [375], [163], other, '( next ( cell 2 2 x ) )'),
    (173, -1, PROPOSITION, [], [225, 409, 264, 44, 101, 46, 441, 269, 460, 271, 417, 542, 94, 174, 399], input, '( does x ( mark 3 2 ) )'),
    (174, -1, AND, [316, 173, 531, 518], [202]),
    (175, -1, AND, [7, 500, 525, 528], [347]),
    (176, -1, OR, [62, 268, 531], [294]),
    (177, -1, AND, [260, 84, 416], [211]),
    (178, -1, PROPOSITION, [383], [553, 376], other, '( sees_tmp3 3 o )'),
    (179, -1, AND, [404, 192, 236], [437]),
    (180, -1, AND, [57, 301], [190]),
    (181, -1, PROPOSITION, [534], [368], other, '( next ( cell 2 2 b ) )'),
    (182, -1, AND, [492, 234], [102]),
    (183, -1, AND, [62, 238, 105], [83]),
    (184, -1, PROPOSITION, [373], [299], other, '( next ( tried o 1 1 ) )'),
    (185, -1, NOT, [266], [42]),
    (186, -1, OR, [402, 63, 50, 262, 121, 59], [99]),
    (187, -1, TRANSITION, [314], [16]),
    (188, -1, PROPOSITION, [365], [506], other, '( next ( tried x 2 1 ) )'),
    (189, -1, PROPOSITION, [26], [6], other, '( next ( cell 2 3 b ) )'),
    (190, -1, OR, [180, 354, 126], [380]),
    (191, -1, PROPOSITION, [501], [], legal, '( legal o ( mark 2 1 ) )'),
    (192, -1, PROPOSITION, [], [512, 551, 164, 302, 271, 251, 5, 307, 364, 209, 169, 447, 544, 419, 179], input, '( does o ( mark 3 1 ) )'),
    (193, -1, OR, [77, 34, 87, 113, 430, 198], [51]),
    (194, -1, NOT, [199], [412]),
    (195, -1, PROPOSITION, [397], [257, 555], other, '( sees_tmp3 1 x )'),
    (196, -1, PROPOSITION, [19], [12, 256, 38, 242], other, '( sees_tmp7 1 )'),
    (197, -1, AND, [7, 500, 528, 518], [45]),
    (198, -1, AND, [82, 425, 0], [193]),
    (199, -1, PROPOSITION, [151], [331, 194], base, '( true ( tried o 2 3 ) )'),
    (200, -1, AND, [492, 472], [28]),
    (201, -1, PROPOSITION, [533], [], legal, '( legal x ( mark 2 1 ) )'),
    (202, -1, OR, [225, 417, 44, 94, 174, 91], [387]),
    (203, -1, OR, [471, 207], [377]),
    (204, -1, OR, [500, 137, 207], [93]),
    (205, -1, AND, [14, 294, 471], [80]),
    (206, -1, AND, [500, 7, 233], [347]),
    (207, -1, PROPOSITION, [], [510, 203, 204, 271, 167, 307, 310, 385, 495, 540, 278, 371, 543, 115, 240], input, '( does o ( mark 3 3 ) )'),
    (208, -1, NOT, [16], [394]),
    (209, -1, AND, [236, 192], [442]),
    (210, -1, AND, [14, 471], [333]),
    (211, -1, OR, [382, 401, 275, 245, 496, 177], [348]),
    (212, -1, PROPOSITION, [45], [148], other, '( next ( cell 1 3 x ) )'),
    (213, -1, PROPOSITION, [303], [169, 429], base, '( true ( tried o 3 1 ) )'),
    (214, -1, NOT, [143], [550]),
    (215, -1, PROPOSITION, [462], [321], other, '( row 3 o )'),
    (216, -1, AND, [129, 234], [102]),
    (217, -1, TRANSITION, [387], [91]),
    (218, -1, AND, [500, 7, 404], [347]),
    (219, -1, PROPOSITION, [120], [72], other, '( next ( cell 1 3 b ) )'),
    (220, -1, AND, [451, 3], [28]),
    (221, -1, TRANSITION, [170], [222]),
    (222, -1, PROPOSITION, [221], [462, 351, 338], base, '( true ( cell 3 2 o ) )'),
    (223, -1, PROPOSITION, [524], [], goal, '( goal o 50 )'),
    (224, -1, AND, [14, 432], [320]),
    (225, -1, AND, [316, 482, 173], [202]),
    (226, -1, AND, [425, 14, 471], [80]),
    (227, -1, AND, [41, 455, 105], [473]),
    (228, -1, PROPOSITION, [90], [408, 27], base, '( true ( tried o 2 2 ) )'),
    (229, -1, PROPOSITION, [422], [17, 337, 489, 127], other, '( sees_tmp7 2 )'),
    (230, -1, OR, [134, 388], [270]),
    (231, -1, AND, [123, 455, 294], [522]),
    (232, -1, AND, [316, 154], [548]),
    (233, -1, PROPOSITION, [104], [402, 434, 452, 540, 288, 206], other, '( next_tmp7 1 )'),
    (234, -1, PROPOSITION, [297], [182, 216], other, '( sees_tmp3 2 x )'),
    (235, -1, OR, [528, 416, 471], [279]),
    (236, -1, PROPOSITION, [152], [25, 551, 454, 249, 456, 302, 88, 363, 251, 5, 413, 209, 565, 254, 447, 544, 419, 179], base, '( true ( cell 3 1 b ) )'),
    (237, -1, OR, [248, 543, 447], [395]),
    (238, -1, PROPOSITION, [334], [450, 420, 534, 319, 183, 484, 288, 128, 463, 561, 413, 273, 311, 344, 66, 568, 258, 69], base, '( true ( cell 2 2 b ) )'),
    (239, -1, TRANSITION, [86], [81]),
    (240, -1, AND, [14, 207], [442]),
    (241, -1, TRANSITION, [117], [0]),
    (242, -1, AND, [196, 571], [28]),
    (243, -1, TRANSITION, [511], [554]),
    (244, -1, PROPOSITION, [352], [12, 369], other, '( sees_tmp1 2 x )'),
    (245, -1, AND, [84, 416, 294], [211]),
    (246, -1, AND, [315, 153, 301], [436]),
    (247, -1, NOT, [545], [563]),
    (248, -1, AND, [316, 531], [237]),
    (249, -1, AND, [15, 236], [515]),
    (250, -1, PROPOSITION, [79], [374, 517], base, '( true ( tried o 1 3 ) )'),
    (251, -1, AND, [236, 192, 73, 518], [54]),
    (252, -1, OR, [409, 88, 156], [131]),
    (253, -1, PROPOSITION, [443], [], legal, '( legal x ( mark 2 3 ) )'),
    (254, -1, AND, [482, 236, 73], [54]),
    (255, -1, AND, [382, 64, 283], [546]),
    (256, -1, AND, [196, 395], [28]),
    (257, -1, AND, [451, 195], [102]),
    (258, -1, AND, [324, 62, 525, 238], [83]),
    (259, -1, OR, [328, 505, 85], [406]),
    (260, -1, PROPOSITION, [307], [8, 355, 484, 430, 177, 289], other, '( next_tmp1 3 )'),
    (261, -1, NOT, [330], [439]),
    (262, -1, AND, [525, 84, 416, 137], [186]),
    (263, -1, AND, [23, 298], [138]),
    (264, -1, AND, [316, 173], [297]),
    (265, -1, OR, [320, 171], [30]),
    (266, -1, PROPOSITION, [431], [185], other, '( marked 1 1 )'),
    (267, -1, OR, [324, 416, 123], [404]),
    (268, -1, PROPOSITION, [], [452, 513, 354, 101, 290, 10, 33, 526, 52, 567, 480, 19, 113, 176, 98], input, '( does o ( mark 1 2 ) )'),
    (269, -1, OR, [173, 554], [511]),
    (270, -1, PROPOSITION, [230], [], goal, '( goal x 50 )'),
    (271, -1, OR, [192, 173, 471, 73, 531, 207], [572]),
    (272, -1, PROPOSITION, [304], [469], other, '( next ( tried x 3 1 ) )'),
    (273, -1, AND, [324, 62, 238, 518], [375]),
    (274, -1, OR, [82, 153, 528], [105]),
    (275, -1, AND, [84, 416, 137, 518], [211]),
    (276, -1, TRANSITION, [277], [124]),
    (277, -1, PROPOSITION, [504], [276], other, '( next ( tried x 1 3 ) )'),
    (278, -1, AND, [14, 471, 207, 518], [80]),
    (279, -1, PROPOSITION, [235], [450, 391, 393, 567, 323, 419], other, '( next_tmp7 3 )'),
    (280, -1, AND, [84, 416], [108]),
    (281, -1, TRANSITION, [346], [157]),
    (282, -1, AND, [7, 500], [383]),
    (283, -1, PROPOSITION, [335], [522, 384, 255], base, '( true ( cell 2 1 x ) )'),
    (284, -1, PROPOSITION, [457], [], legal, '( legal x ( mark 1 3 ) )'),
    (285, -1, TRANSITION, [308], [143]),
    (286, -1, AND, [451, 100], [102]),
    (287, -1, PROPOSITION, [396], [], legal, '( legal x ( mark 2 2 ) )'),
    (288, -1, AND, [62, 233, 238], [83]),
    (289, -1, AND, [260, 123, 455], [522]),
    (290, -1, OR, [500, 57, 268], [482]),
    (291, -1, PROPOSITION, [517], [79], other, '( next ( tried o 1 3 ) )'),
    (292, -1, OR, [471, 157], [346]),
    (293, -1, AND, [84, 137], [383]),
    (294, -1, PROPOSITION, [176], [326, 245, 312, 454, 205, 231], other, '( next_tmp3 2 )'),
    (295, -1, TRANSITION, [51], [34]),
    (296, -1, PROPOSITION, [493], [538, 444, 367, 80], base, '( true ( cell 3 3 x ) )'),
    (297, -1, OR, [561, 160, 264], [234]),
    (298, -1, NOT, [459], [353, 134, 263, 336]),
    (299, -1, TRANSITION, [184], [340]),
    (300, -1, TRANSITION, [70], [166]),
    (301, -1, PROPOSITION, [529], [328, 180, 40, 246, 42, 355, 145, 125, 323, 361, 4, 559, 413, 326, 309, 97, 448, 139], base, '( true ( cell 1 1 b ) )'),
    (302, -1, AND, [192, 236, 105], [437]),
    (303, -1, TRANSITION, [65], [213]),
    (304, -1, OR, [464, 73], [272]),
    (305, -1, AND, [500, 7, 1], [347]),
    (306, -1, PROPOSITION, [440], [], goal, '( goal o 0 )'),
    (307, -1, OR, [192, 531, 207], [260]),
    (308, -1, PROPOSITION, [510], [285], other, '( next ( tried o 3 3 ) )'),
    (309, -1, AND, [153, 301], [333]),
    (310, -1, AND, [14, 105, 207], [357]),
    (311, -1, AND, [324, 482, 238], [375]),
    (312, -1, AND, [7, 528, 294], [45]),
    (313, -1, AND, [7, 528], [333]),
    (314, -1, PROPOSITION, [67], [187], other, '( next ( tried x 1 2 ) )'),
    (315, -1, PROPOSITION, [75], [168, 77, 246, 94, 499, 456], other, '( next_tmp1 2 )'),
    (316, -1, PROPOSITION, [322], [140, 434, 225, 393, 409, 248, 264, 44, 46, 441, 165, 232, 413, 132, 417, 94, 174, 532], base, '( true ( cell 3 2 b ) )'),
    (317, -1, NOT, [53], [26]),
    (318, -1, PROPOSITION, [390], [], legal, '( legal o ( mark 1 2 ) )'),
    (319, -1, AND, [62, 238], [442]),
    (320, -1, PROPOSITION, [224], [265], other, '( next ( cell 3 3 b ) )'),
    (321, -1, OR, [521, 453, 112, 215, 21, 60, 558], [23]),
    (322, -1, TRANSITION, [378], [316]),
    (323, -1, AND, [57, 301, 279], [562]),
    (324, -1, PROPOSITION, [], [435, 484, 267, 101, 422, 463, 561, 477, 273, 311, 66, 568, 258, 399, 69], input, '( does x ( mark 2 2 ) )'),
    (325, -1, OR, [538, 147], [370]),
    (326, -1, AND, [153, 301, 294], [436]),
    (327, -1, OR, [416, 137], [53]),
    (328, -1, AND, [153, 301], [259]),
    (329, -1, PROPOSITION, [367], [78], other, '( row 3 x )'),
    (330, -1, PROPOSITION, [13], [261], other, '( marked 2 1 )'),
    (331, -1, OR, [137, 199], [539]),
    (332, -1, PROPOSITION, [519], [537], other, '( next ( tried x 2 3 ) )'),
    (333, -1, OR, [25, 144, 309, 210, 161, 568, 389, 313, 441], [424]),
    (334, -1, TRANSITION, [368], [238]),
    (335, -1, TRANSITION, [481], [283]),
    (336, -1, AND, [23, 298], [400]),
    (337, -1, AND, [131, 229], [102]),
    (338, -1, AND, [222, 427, 166], [453]),
    (339, -1, PROPOSITION, [37], [110], other, '( next ( tried x 1 1 ) )'),
    (340, -1, PROPOSITION, [299], [373, 103], base, '( true ( tried o 1 1 ) )'),
    (341, -1, PROPOSITION, [357], [514], other, '( next ( cell 3 3 o ) )'),
    (342, -1, PROPOSITION, [414], [359], other, '( next ( cell 1 2 o ) )'),
    (343, -1, PROPOSITION, [349], [], legal, '( legal random ( tiebreak o ) )'),
    (344, -1, AND, [62, 238], [31]),
    (345, -1, PROPOSITION, [347], [449], other, '( next ( cell 1 3 o ) )'),
    (346, -1, PROPOSITION, [292], [281], other, '( next ( tried x 3 3 ) )'),
    (347, -1, OR, [305, 49, 421, 175, 206, 218], [345]),
    (348, -1, PROPOSITION, [211], [155], other, '( next ( cell 2 3 x ) )'),
    (349, 1, CONSTANT, [], [43, 343, 466]),
    (350, -1, PROPOSITION, [408], [], legal, '( legal o ( mark 2 2 ) )'),
    (351, -1, OR, [434, 222, 132, 393, 46, 165], [170]),
    (352, -1, OR, [476, 463, 503], [244]),
    (353, -1, AND, [520, 502, 298], [524]),
    (354, -1, AND, [0, 268], [190]),
    (355, -1, AND, [260, 153, 301], [436]),
    (356, -1, AND, [41, 455], [442]),
    (357, -1, OR, [167, 109, 310, 540, 495, 371], [341]),
    (358, -1, AND, [23, 459], [524]),
    (359, -1, TRANSITION, [342], [427]),
    (360, -1, AND, [50, 470, 166], [112]),
    (361, -1, AND, [153, 57, 301, 518], [436]),
    (362, -1, PROPOSITION, [148], [549, 444, 45, 147], base, '( true ( cell 1 3 x ) )'),
    (363, -1, AND, [236, 73], [397]),
    (364, -1, OR, [41, 57, 192], [425]),
    (365, -1, OR, [509, 123], [188]),
    (366, -1, AND, [424, 518], [102]),
    (367, -1, AND, [372, 296, 91], [329]),
    (368, -1, OR, [181, 171], [334]),
    (369, -1, AND, [244, 572], [102]),
    (370, -1, PROPOSITION, [325], [78], other, '( diagonal x )'),
    (371, -1, AND, [14, 525, 471, 207], [357]),
    (372, -1, PROPOSITION, [20], [384, 367, 147, 54], base, '( true ( cell 3 1 x ) )'),
    (373, -1, OR, [340, 57], [184]),
    (374, -1, NOT, [250], [541]),
    (375, -1, OR, [273, 64, 311, 66, 484, 69], [172]),
    (376, -1, AND, [451, 178], [28]),
    (377, -1, PROPOSITION, [203], [432], other, '( marked 3 3 )'),
    (378, -1, OR, [548, 171], [322]),
    (379, -1, PROPOSITION, [507], [89], other, '( next ( tried o 2 1 ) )'),
    (380, -1, PROPOSITION, [190], [142, 127], other, '( sees_tmp1 1 o )'),
    (381, -1, PROPOSITION, [513], [116], other, '( next ( tried o 1 2 ) )'),
    (382, -1, PROPOSITION, [155], [444, 211, 255], base, '( true ( cell 2 3 x ) )'),
    (383, -1, OR, [293, 282, 115], [178]),
    (384, -1, AND, [158, 283, 372], [398]),
    (385, -1, OR, [500, 528, 416, 137, 471, 207], [492]),
    (386, -1, TRANSITION, [527], [470]),
    (387, -1, PROPOSITION, [202], [217], other, '( next ( cell 3 2 x ) )'),
    (388, -1, AND, [23, 459], [230]),
    (389, -1, AND, [84, 416], [333]),
    (390, -1, NOT, [497], [318]),
    (391, -1, AND, [41, 455, 279], [473]),
    (392, -1, OR, [531, 545], [547]),
    (393, -1, AND, [316, 531, 279], [351]),
    (394, -1, PROPOSITION, [208], [], legal, '( legal x ( mark 1 2 ) )'),
    (395, -1, PROPOSITION, [237], [256, 489], other, '( sees_tmp1 3 o )'),
    (396, -1, NOT, [81], [287]),
    (397, -1, OR, [363, 125, 107], [195]),
    (398, -1, PROPOSITION, [384], [78], other, '( column 1 x )'),
    (399, -1, OR, [324, 82, 173], [426]),
    (400, -1, PROPOSITION, [336], [], goal, '( goal x 0 )'),
    (401, -1, AND, [425, 84, 416], [211]),
    (402, -1, AND, [233, 84, 137], [186]),
    (403, -1, NOT, [428], [96]),
    (404, -1, PROPOSITION, [267], [132, 495, 448, 98, 179, 218], other, '( next_tmp5 2 )'),
    (405, -1, AND, [84, 137], [162]),
    (406, -1, PROPOSITION, [259], [17, 498], other, '( sees_tmp1 1 x )'),
    (407, -1, PROPOSITION, [562], [566], other, '( next ( cell 1 1 o ) )'),
    (408, -1, NOT, [228], [350]),
    (409, -1, AND, [316, 173], [252]),
    (410, -1, AND, [118, 49, 166], [569]),
    (411, -1, OR, [130, 171], [529]),
    (412, -1, PROPOSITION, [194], [], legal, '( legal o ( mark 2 3 ) )'),
    (413, -1, OR, [7, 316, 14, 0, 236, 84, 301, 238, 455], [570]),
    (414, -1, OR, [452, 427, 52, 567, 480, 98], [342]),
    (415, -1, AND, [7, 425, 528], [45]),
    (416, -1, PROPOSITION, [], [401, 245, 262, 267, 422, 519, 476, 235, 275, 385, 496, 327, 389, 177, 280], input, '( does x ( mark 2 3 ) )'),
    (417, -1, AND, [316, 93, 173], [202]),
    (418, -1, NOT, [157], [149]),
    (419, -1, AND, [192, 236, 279], [437]),
    (420, -1, AND, [62, 1, 238], [83]),
    (421, -1, AND, [500, 7, 426], [347]),
    (422, -1, OR, [324, 62, 41, 416, 137, 123], [229]),
    (423, -1, PROPOSITION, [494], [], legal, '( legal x ( mark 1 1 ) )'),
    (424, -1, PROPOSITION, [333], [366], other, '( sees_tmp5 x )'),
    (425, -1, PROPOSITION, [364], [401, 226, 415, 44, 198, 69], other, '( next_tmp3 1 )'),
    (426, -1, PROPOSITION, [399], [167, 5, 63, 55, 421, 559], other, '( next_tmp7 2 )'),
    (427, -1, PROPOSITION, [359], [414, 458, 338], base, '( true ( cell 1 2 o ) )'),
    (428, -1, PROPOSITION, [33], [403], other, '( marked 1 2 )'),
    (429, -1, NOT, [213], [438]),
    (430, -1, AND, [82, 260, 0], [193]),
    (431, -1, OR, [153, 57], [266]),
    (432, -1, NOT, [377], [224]),
    (433, -1, PROPOSITION, [566], [562, 536, 458, 461], base, '( true ( cell 1 1 o ) )'),
    (434, -1, AND, [233, 316, 531], [351]),
    (435, -1, OR, [324, 62], [485]),
    (436, -1, OR, [158, 326, 246, 355, 97, 361], [141]),
    (437, -1, OR, [5, 118, 302, 419, 544, 179], [48]),
    (438, -1, PROPOSITION, [429], [], legal, '( legal o ( mark 3 1 ) )'),
    (439, -1, AND, [261, 455], [150]),
    (440, -1, AND, [502, 459], [306]),
    (441, -1, AND, [316, 173], [333]),
    (442, -1, OR, [475, 209, 10, 319, 356, 58, 240, 4, 532], [468]),
    (443, -1, NOT, [92], [253]),
    (444, -1, AND, [382, 296, 362], [119]),
    (445, -1, AND, [109, 49, 50], [521]),
    (446, -1, AND, [14, 471], [108]),
    (447, -1, AND, [236, 192], [237]),
    (448, -1, AND, [404, 57, 301], [562]),
    (449, -1, TRANSITION, [345], [49]),
    (450, -1, AND, [62, 238, 279], [83]),
    (451, -1, PROPOSITION, [101], [220, 286, 257, 376], other, '( sees_tmp9 2 )'),
    (452, -1, AND, [233, 0, 268], [414]),
    (453, -1, PROPOSITION, [338], [321], other, '( column 2 o )'),
    (454, -1, AND, [236, 294, 73], [54]),
    (455, -1, PROPOSITION, [146], [55, 503, 391, 227, 535, 161, 356, 439, 289, 47, 107, 231, 413, 465, 18, 68, 114, 467], base, '( true ( cell 2 1 b ) )'),
    (456, -1, AND, [315, 236, 73], [54]),
    (457, -1, NOT, [124], [284]),
    (458, -1, AND, [427, 49, 433], [60]),
    (459, -1, PROPOSITION, [78], [24, 478, 298, 358, 440, 388], other, '( line x )'),
    (460, -1, OR, [173, 471, 73], [1]),
    (461, -1, AND, [118, 470, 433], [21]),
    (462, -1, AND, [118, 222, 109], [215]),
    (463, -1, AND, [324, 238], [352]),
    (464, -1, PROPOSITION, [469], [483, 304], base, '( true ( tried x 3 1 ) )'),
    (465, -1, AND, [93, 123, 455], [522]),
    (466, -1, PROPOSITION, [349], [], goal, '( goal random 0 )'),
    (467, -1, AND, [41, 455, 123, 518], [522]),
    (468, -1, PROPOSITION, [442], [552], other, '( sees_tmp5 o )'),
    (469, -1, TRANSITION, [272], [464]),
    (470, -1, PROPOSITION, [386], [360, 473, 461], base, '( true ( cell 2 1 o ) )'),
    (471, -1, PROPOSITION, [], [226, 203, 516, 205, 460, 271, 235, 292, 385, 210, 278, 371, 446, 499, 156], input, '( does x ( mark 3 3 ) )'),
    (472, -1, PROPOSITION, [31], [200, 133], other, '( sees_tmp3 2 o )'),
    (473, -1, OR, [55, 391, 227, 535, 68, 470], [527]),
    (474, -1, PROPOSITION, [483], [], legal, '( legal x ( mark 3 1 ) )'),
    (475, -1, AND, [7, 500], [442]),
    (476, -1, AND, [84, 416], [352]),
    (477, -1, OR, [81, 324], [86]),
    (478, -1, OR, [23, 520, 459], [95]),
    (479, -1, TRANSITION, [48], [118]),
    (480, -1, AND, [0, 1, 268], [414]),
    (481, -1, PROPOSITION, [522], [335], other, '( next ( cell 2 1 x ) )'),
    (482, -1, PROPOSITION, [290], [225, 311, 496, 254, 516, 114], other, '( next_tmp1 1 )'),
    (483, -1, NOT, [464], [474]),
    (484, -1, AND, [324, 260, 238], [375]),
    (485, -1, PROPOSITION, [435], [556], other, '( marked 2 2 )'),
    (486, -1, AND, [129, 100], [102]),
    (487, -1, PROPOSITION, [27], [90], other, '( next ( tried o 2 2 ) )'),
    (488, -1, PROPOSITION, [122], [78], other, '( column 2 x )'),
    (489, -1, AND, [229, 395], [28]),
    (490, -1, TRANSITION, [99], [50]),
    (491, -1, TRANSITION, [141], [158]),
    (492, -1, PROPOSITION, [385], [200, 523, 182, 555], other, '( sees_tmp9 3 )'),
    (493, -1, TRANSITION, [61], [296]),
    (494, -1, NOT, [71], [423]),
    (495, -1, AND, [404, 14, 207], [357]),
    (496, -1, AND, [482, 84, 416], [211]),
    (497, -1, PROPOSITION, [116], [513, 390], base, '( true ( tried o 1 2 ) )'),
    (498, -1, AND, [406, 572], [102]),
    (499, -1, AND, [315, 14, 471], [80]),
    (500, -1, PROPOSITION, [], [282, 421, 204, 126, 517, 206, 475, 290, 305, 385, 197, 135, 19, 175, 218], input, '( does o ( mark 1 3 ) )'),
    (501, -1, NOT, [22], [191]),
    (502, -1, NOT, [23], [24, 353, 134, 440]),
    (503, -1, AND, [455, 123], [352]),
    (504, -1, OR, [528, 124], [277]),
    (505, -1, AND, [7, 528], [259]),
    (506, -1, TRANSITION, [188], [509]),
    (507, -1, OR, [41, 22], [379]),
    (508, -1, NOT, [554], [159]),
    (509, -1, PROPOSITION, [506], [533, 365], base, '( true ( tried x 2 1 ) )'),
    (510, -1, OR, [143, 207], [308]),
    (511, -1, PROPOSITION, [269], [243], other, '( next ( tried x 3 2 ) )'),
    (512, -1, OR, [192, 73], [530]),
    (513, -1, OR, [497, 268], [381]),
    (514, -1, TRANSITION, [341], [109]),
    (515, -1, PROPOSITION, [249], [74], other, '( next ( cell 3 1 b ) )'),
    (516, -1, AND, [482, 14, 471], [80]),
    (517, -1, OR, [500, 250], [291]),
    (518, -1, PROPOSITION, [], [251, 273, 275, 366, 278, 197, 174, 113, 361, 467], input, '( does random ( tiebreak x ) )'),
    (519, -1, OR, [416, 92], [332]),
    (520, -1, NOT, [570], [478, 353, 134]),
    (521, -1, PROPOSITION, [445], [321], other, '( column 3 o )'),
    (522, -1, OR, [465, 283, 289, 114, 467, 231], [481]),
    (523, -1, AND, [492, 3], [28]),
    (524, -1, OR, [353, 358], [223]),
    (525, -1, PROPOSITION, [], [262, 52, 552, 68, 371, 258, 46, 175, 544, 139], input, '( does random ( tiebreak o ) )'),
    (526, -1, AND, [0, 268], [31]),
    (527, -1, PROPOSITION, [473], [386], other, '( next ( cell 2 1 o ) )'),
    (528, -1, PROPOSITION, [], [504, 505, 168, 8, 235, 274, 415, 385, 197, 135, 312, 19, 136, 175, 313], input, '( does x ( mark 1 3 ) )'),
    (529, -1, TRANSITION, [411], [301]),
    (530, -1, PROPOSITION, [512], [15], other, '( marked 3 1 )'),
    (531, -1, PROPOSITION, [], [140, 434, 392, 393, 248, 101, 46, 165, 271, 307, 132, 542, 174, 176, 532], input, '( does o ( mark 3 2 ) )'),
    (532, -1, AND, [316, 531], [442]),
    (533, -1, NOT, [509], [201]),
    (534, -1, AND, [238, 556], [181]),
    (535, -1, AND, [41, 1, 455], [473]),
    (536, -1, AND, [109, 433, 166], [569]),
    (537, -1, TRANSITION, [332], [92]),
    (538, -1, AND, [158, 64, 296], [325]),
    (539, -1, PROPOSITION, [331], [151], other, '( next ( tried o 2 3 ) )'),
    (540, -1, AND, [233, 14, 207], [357]),
    (541, -1, PROPOSITION, [374], [], legal, '( legal o ( mark 1 3 ) )'),
    (542, -1, OR, [173, 531], [2]),
    (543, -1, AND, [14, 207], [237]),
    (544, -1, AND, [525, 236, 192, 73], [437]),
    (545, -1, PROPOSITION, [56], [392, 247], base, '( true ( tried o 3 2 ) )'),
    (546, -1, PROPOSITION, [255], [78], other, '( row 2 x )'),
    (547, -1, PROPOSITION, [392], [56], other, '( next ( tried o 3 2 ) )'),
    (548, -1, PROPOSITION, [232], [378], other, '( next ( cell 3 2 b ) )'),
    (549, -1, AND, [158, 34, 362], [35]),
    (550, -1, PROPOSITION, [214], [], legal, '( legal o ( mark 3 3 ) )'),
    (551, -1, AND, [236, 192], [11]),
    (552, -1, AND, [468, 525], [28]),
    (553, -1, AND, [129, 178], [28]),
    (554, -1, PROPOSITION, [243], [508, 269], base, '( true ( tried x 3 2 ) )'),
    (555, -1, AND, [492, 195], [102]),
    (556, -1, NOT, [485], [534]),
    (557, -1, PROPOSITION, [96], [117], other, '( next ( cell 1 2 b ) )'),
    (558, -1, PROPOSITION, [569], [321], other, '( diagonal o )'),
    (559, -1, AND, [426, 57, 301], [562]),
    (560, -1, PROPOSITION, [54], [20], other, '( next ( cell 3 1 x ) )'),
    (561, -1, AND, [324, 238], [297]),
    (562, -1, OR, [145, 448, 323, 433, 139, 559], [407]),
    (563, -1, PROPOSITION, [247], [], legal, '( legal o ( mark 3 2 ) )'),
    (564, -1, AND, [571, 572], [28]),
    (565, -1, AND, [236, 93, 73], [54]),
    (566, -1, TRANSITION, [407], [433]),
    (567, -1, AND, [0, 268, 279], [414]),
    (568, -1, AND, [324, 238], [333]),
    (569, -1, OR, [536, 410], [558]),
    (570, -1, PROPOSITION, [413], [520], other, 'open'),
    (571, -1, PROPOSITION, [162], [564, 242], other, '( sees_tmp1 2 o )'),
    (572, -1, PROPOSITION, [271], [142, 564, 369, 498], other, '( sees_tmp7 3 )'),
    (573, -1, PROPOSITION, [103], [], legal, '( legal o ( mark 1 1 ) )'),
)

# DONE
