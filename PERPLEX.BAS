'PERPLEX DELUXE ver 1.0  COPYRIGHT 1991 RAY VANDERBORGHT-CONCEPT BY MATT AHOLA

INIT:
SCREEN 13
IF TT = 1 THEN GOTO START

CLS
DIM GB(-2 TO 12, -2 TO 12) 'GAME BOARD ARRAY
DIM Perplex(1 TO 910) AS INTEGER 'LOGO IN CORNER
DIM TPLEX(1 TO 7106) AS INTEGER 'PERPLEX TITLE IMAGE

DIM CR(1 TO 339) AS INTEGER 'CURSER PICTURE
DIM BA(1 TO 339) AS INTEGER 'BARRIER PICTURE
DIM RP(1 TO 339) AS INTEGER 'RED PIECE PICTURE
DIM BP(1 TO 339) AS INTEGER 'BLUE PIECE PICTURE
DIM BRP(1 TO 339) AS INTEGER 'BIG RED PIECE
DIM SRP(1 TO 339) AS INTEGER 'SMALL RED PIECE
DIM BBP(1 TO 339) AS INTEGER 'BIG BLUE PIECE
DIM SBP(1 TO 339) AS INTEGER 'SMALL BLUE PIECE
DIM RL(1 TO 314) AS INTEGER 'RED LIGHT
DIM BL(1 TO 314) AS INTEGER 'BLUE LIGHT

DIM SO(1 TO 760) AS INTEGER 'SOUND SIGN
DIM FO(1 TO 760) AS INTEGER 'FORFIET SIGN
DIM AO(1 TO 760) AS INTEGER 'PLAY AGAIN SIGN
DIM PO(1 TO 760) AS INTEGER 'PASS SIGN
DIM QO(1 TO 760) AS INTEGER 'QUIT SIGN
DIM TIE(1 TO 760) AS INTEGER 'TIE SIGN
DIM RWIN(1 TO 760) AS INTEGER 'RED WIN SIGN
DIM BWIN(1 TO 760) AS INTEGER 'BLUE WIN SIGN
DIM PLYR(1 TO 760) AS INTEGER '1 OR 2 PLAYERS

DIM D0(1 TO 402) AS INTEGER
DIM D1(1 TO 402) AS INTEGER
DIM D2(1 TO 402) AS INTEGER
DIM D3(1 TO 402) AS INTEGER
DIM D4(1 TO 402) AS INTEGER
DIM D5(1 TO 402) AS INTEGER
DIM D6(1 TO 402) AS INTEGER
DIM D7(1 TO 402) AS INTEGER
DIM D8(1 TO 402) AS INTEGER
DIM D9(1 TO 402) AS INTEGER

DIM RBOX(1 TO 1326) AS INTEGER
DIM BBOX(1 TO 1298) AS INTEGER

DIM WDW(1 TO 800) 'WINDOW

GOSUB TITLE 'GET TITLE MUSIC + JUNK

START:
CLS
A = 1: B = 1 'STARTING LOCATION FOR CURSER      (A AND B ARE 1 - 8)
X = 5: Y = 5 'STARTING PLACE FOR CURSER         (X AND Y ARE 30,24)
CO = 1 'RED STARTS

'PRINT STUFF ON THE SCREEN
PUT (250, 5), Perplex(), PSET
PUT (256, 101), RBOX()
PUT (256, 150), BBOX()
PUT (268, 55), RL(), PSET
PUT (260, 110), D0(), PSET
PUT (285, 110), D2(), PSET
PUT (260, 159), D0(), PSET
PUT (285, 159), D2(), PSET

'DRAW THE GRID
FOR i = 3 TO 247 STEP 30
    FOR J = 3 TO 199 STEP 24
        LINE (1 + i, 1 + J)-(247 - i, 199 - J), 1, B
        LINE (1, 1)-(319, 199), 8, B
    NEXT
NEXT

'MAKE EDGE OF BOARD IMPASSABLE
FOR i = -1 TO 10
    GB(-1, i) = 4: GB(0, i) = 4: GB(9, i) = 4: GB(10, i) = 4
    GB(i, -1) = 4: GB(i, 0) = 4: GB(i, 9) = 4: GB(i, 10) = 4
NEXT

'PLACE BARRIERS IN ARRAY   GJJBFKDGBDFGSDFGKJBSDFKGJBSKGKJSDGBSDGB
GB(3, 3) = 3: GB(6, 6) = 3: GB(6, 3) = 3: GB(3, 6) = 3

'PLACE PIECES IN ARRAY
GB(1, 1) = 1: GB(8, 8) = 1: GB(1, 8) = 2: GB(8, 1) = 2

'DRAW BARRIERS
PUT (65, 53), BA(): PUT (155, 125), BA(): PUT (155, 53), BA(): PUT (65, 125), BA()

'DRAW PIECES AND CURSER
PUT (5, 5), RP(): PUT (215, 173), RP(): PUT (5, 173), BP(): PUT (215, 5), BP()
PUT (5, 5), CR()


GET (95, 90)-(160, 115), WDW()
PLAYERS:
K$ = INKEY$
PUT (95, 90), PLYR(), PSET
IF K$ = "1" THEN COMP = 1
IF K$ = "2" THEN COMP = 0
IF K$ <> "1" AND K$ <> "2" THEN GOTO PLAYERS
PUT (95, 90), WDW(), PSET

'***************THIS IS THE BEGINNING OF THE MAIN PROGRAM*********************



'-----------RESPOND TO KEYPRESSES---------------------------------------------
CURSER:
K$ = INKEY$
KEY(11) ON
KEY(12) ON
KEY(13) ON
KEY(14) ON
ON KEY(11) GOSUB CRU
ON KEY(14) GOSUB CRD
ON KEY(12) GOSUB CRL
ON KEY(13) GOSUB CRR

IF K$ = CHR$(32) AND CO = 1 AND GB(A, B) = 1 AND P = 1 THEN
    PUT (X1, Y1), BRP(): PUT (X1, Y1), RP(): PUT (X1, Y1), CR()
END IF

IF K$ = CHR$(32) AND CO = 2 AND GB(A, B) = 2 AND P = 1 THEN
    PUT (X1, Y1), BBP(): PUT (X1, Y1), BP(): PUT (X1, Y1), CR()
END IF

IF K$ = CHR$(32) AND CO = 1 AND GB(A, B) = 1 THEN GOTO PICK 'SELECT W/SPACE
IF K$ = CHR$(32) AND CO = 2 AND GB(A, B) = 2 THEN GOTO PICK 'SELECT W/SPACE

IF K$ = CHR$(32) AND P = 1 AND GB(A, B) = 0 THEN GOTO RULES 'WHERE TO MOVE

IF K$ = "P" OR K$ = "p" THEN GOSUB OPTPASS: GOTO CURSER
IF K$ = "S" OR K$ = "s" THEN GOSUB OPTSOUND: GOTO CURSER
IF K$ = "F" OR K$ = "f" THEN GOSUB OPTFOR: GOTO INIT

IF CO = 1 THEN PUT (268, 55), RL(), PSET ELSE PUT (268, 55), BL(), PSET
IF COMP = 1 AND CO = 2 THEN GOTO COMPUTER
IF K$ = CHR$(27) THEN GOSUB OPTQUIT
GOTO CURSER


'-------------SAVE LOCATION---------------------------------------------------
PICK:
X1 = X: Y1 = Y
A1 = A: B1 = B

IF CO = 1 THEN
    GOSUB KEYOFF
    FOR PLS = 1 TO 2
        PUT (X, Y), BRP(), PSET: FOR WW = 1 TO 225: NEXT: 'BIG RED PIECE
        delay (1) ' #2011: replacing this - FOR i = 1 TO 100: NEXT

        PUT (X, Y), RP(), PSET: FOR WW = 1 TO 225: NEXT: 'RED PIECE (REGULAR)
        delay (1) ' #2011: replacing this - FOR i = 1 TO 100: NEXT

        PUT (X, Y), SRP(), PSET: FOR WW = 1 TO 225: NEXT: 'SMALL RED PIECE
        delay (1) ' #2011: replacing this - FOR i = 1 TO 100: NEXT

        PUT (X, Y), RP(), PSET: FOR WW = 1 TO 225: NEXT: 'RED PIECE (REGULAR)
        delay (1) ' #2011: replacing this - FOR i = 1 TO 100: NEXT

        PUT (X, Y), BRP(), PSET: FOR WW = 1 TO 225: NEXT: 'BIG RED PIECE
        delay (1) ' #2011: replacing this - FOR i = 1 TO 100: NEXT
    NEXT
END IF

IF CO = 2 THEN
    GOSUB KEYOFF
    FOR PLS = 1 TO 2
        PUT (X, Y), BBP(), PSET: FOR WW = 1 TO 225: NEXT: 'BIG BLUE PIECE
        delay (1) ' #2011: replacing this - FOR i = 1 TO 100: NEXT

        PUT (X, Y), BP(), PSET: FOR WW = 1 TO 225: NEXT: 'BLUE PIECE (REGULAR)
        delay (1) ' #2011: replacing this - FOR i = 1 TO 100: NEXT

        PUT (X, Y), SBP(), PSET: FOR WW = 1 TO 225: NEXT: 'SMALL BLUE PIECE
        delay (1) ' #2011: replacing this - FOR i = 1 TO 100: NEXT

        PUT (X, Y), BP(), PSET: FOR WW = 1 TO 225: NEXT: 'BLUE PIECE (REGULAR)
        delay (1) ' #2011: replacing this - FOR i = 1 TO 100: NEXT

        PUT (X, Y), BBP(), PSET: FOR WW = 1 TO 225: NEXT: 'BIG BLUE PIECE
        delay (1) ' #2011: replacing this - FOR i = 1 TO 100: NEXT
    NEXT
END IF

P = 1
GOTO CURSER


'-----CHECK LEGALITY OF MOVE AND CHOSE WHICH ACTION TO TAKE IF LEGAL----------
RULES:
GOSUB REDUCE
IF A > A1 + 2 THEN P = 0: GOTO CURSER
IF A < A1 - 2 THEN P = 0: GOTO CURSER
IF B > B1 + 2 THEN P = 0: GOTO CURSER
IF B < B1 - 2 THEN P = 0: GOTO CURSER

IF A = A1 + 2 THEN GOTO JUMP
IF A = A1 - 2 THEN GOTO JUMP
IF B = B1 + 2 THEN GOTO JUMP
IF B = B1 - 2 THEN GOTO JUMP

IF A = A1 + 1 THEN GOTO make
IF A = A1 - 1 THEN GOTO make
IF B = B1 + 1 THEN GOTO make
IF B = B1 - 1 THEN GOTO make


'---------GENERATE A PIECE NEXT TO YOUR PIECE---------------------------------
make:
GOSUB KEYOFF
IF CO = 1 THEN PUT (X, Y), RP()
IF CO = 2 THEN PUT (X, Y), BP()
IF CO = 1 THEN GB(A, B) = 1
IF CO = 2 THEN GB(A, B) = 2
GOSUB MS
GOTO CREATE

'---------JUMP A PIECE--------------------------------------------------------
JUMP:
GOSUB KEYOFF
IF CO = 1 THEN PUT (X1, Y1), RP(): PUT (X, Y), RP()
IF CO = 2 THEN PUT (X1, Y1), BP(): PUT (X, Y), BP()
IF CO = 1 THEN GB(A, B) = 1
IF CO = 2 THEN GB(A, B) = 2
GB(A1, B1) = 0
GOSUB JS
GOTO CREATE


'----------PROCEDURE TO EFFECT SURROUNDING PIECES-----------------------------
CREATE:
GOSUB KEYOFF
IF CO = 1 AND GB(A + 1, B + 1) = 2 THEN GB(A + 1, B + 1) = 1: PUT (X + 30, Y + 24), BP(): PUT (X + 30, Y + 24), RP(): GOSUB SS
IF CO = 1 AND GB(A + 0, B + 1) = 2 THEN GB(A + 0, B + 1) = 1: PUT (X + 0, Y + 24), BP(): PUT (X + 0, Y + 24), RP(): GOSUB SS
IF CO = 1 AND GB(A - 1, B + 1) = 2 THEN GB(A - 1, B + 1) = 1: PUT (X - 30, Y + 24), BP(): PUT (X - 30, Y + 24), RP(): GOSUB SS
IF CO = 1 AND GB(A + 1, B + 0) = 2 THEN GB(A + 1, B + 0) = 1: PUT (X + 30, Y + 0), BP(): PUT (X + 30, Y + 0), RP(): GOSUB SS
IF CO = 1 AND GB(A - 1, B + 0) = 2 THEN GB(A - 1, B + 0) = 1: PUT (X - 30, Y + 0), BP(): PUT (X - 30, Y + 0), RP(): GOSUB SS
IF CO = 1 AND GB(A + 1, B - 1) = 2 THEN GB(A + 1, B - 1) = 1: PUT (X + 30, Y - 24), BP(): PUT (X + 30, Y - 24), RP(): GOSUB SS
IF CO = 1 AND GB(A + 0, B - 1) = 2 THEN GB(A + 0, B - 1) = 1: PUT (X + 0, Y - 24), BP(): PUT (X + 0, Y - 24), RP(): GOSUB SS
IF CO = 1 AND GB(A - 1, B - 1) = 2 THEN GB(A - 1, B - 1) = 1: PUT (X - 30, Y - 24), BP(): PUT (X - 30, Y - 24), RP(): GOSUB SS

IF CO = 2 AND GB(A + 1, B + 1) = 1 THEN GB(A + 1, B + 1) = 2: PUT (X + 30, Y + 24), RP(): PUT (X + 30, Y + 24), BP(): GOSUB SS
IF CO = 2 AND GB(A + 0, B + 1) = 1 THEN GB(A + 0, B + 1) = 2: PUT (X + 0, Y + 24), RP(): PUT (X + 0, Y + 24), BP(): GOSUB SS
IF CO = 2 AND GB(A - 1, B + 1) = 1 THEN GB(A - 1, B + 1) = 2: PUT (X - 30, Y + 24), RP(): PUT (X - 30, Y + 24), BP(): GOSUB SS
IF CO = 2 AND GB(A + 1, B + 0) = 1 THEN GB(A + 1, B + 0) = 2: PUT (X + 30, Y + 0), RP(): PUT (X + 30, Y + 0), BP(): GOSUB SS
IF CO = 2 AND GB(A - 1, B + 0) = 1 THEN GB(A - 1, B + 0) = 2: PUT (X - 30, Y + 0), RP(): PUT (X - 30, Y + 0), BP(): GOSUB SS
IF CO = 2 AND GB(A + 1, B - 1) = 1 THEN GB(A + 1, B - 1) = 2: PUT (X + 30, Y - 24), RP(): PUT (X + 30, Y - 24), BP(): GOSUB SS
IF CO = 2 AND GB(A + 0, B - 1) = 1 THEN GB(A + 0, B - 1) = 2: PUT (X + 0, Y - 24), RP(): PUT (X + 0, Y - 24), BP(): GOSUB SS
IF CO = 2 AND GB(A - 1, B - 1) = 1 THEN GB(A - 1, B - 1) = 2: PUT (X - 30, Y - 24), RP(): PUT (X - 30, Y - 24), BP(): GOSUB SS
P = 0
IF CO = 1 THEN CO = 2 ELSE CO = 1
IF COMP = 1 AND CO = 2 THEN GOTO COMPUTER
GOSUB ENDGAME
GOTO CURSER


'------------------CHECK TO SEE IF GAME IS DONE-------------------------------
ENDGAME:
FOR E = 1 TO 8
    FOR F = 1 TO 8
        IF GB(E, F) = 1 THEN RED = RED + 1
        IF GB(E, F) = 2 THEN BLU = BLU + 1
        IF GB(E, F) = 0 THEN NIL = NIL + 1
    NEXT
NEXT

'DIGITAL DISPLAY

'RED
IF RED MOD 10 = 0 THEN PUT (285, 110), D0(), PSET
IF RED MOD 10 = 1 THEN PUT (285, 110), D1(), PSET
IF RED MOD 10 = 2 THEN PUT (285, 110), D2(), PSET
IF RED MOD 10 = 3 THEN PUT (285, 110), D3(), PSET
IF RED MOD 10 = 4 THEN PUT (285, 110), D4(), PSET
IF RED MOD 10 = 5 THEN PUT (285, 110), D5(), PSET
IF RED MOD 10 = 6 THEN PUT (285, 110), D6(), PSET
IF RED MOD 10 = 7 THEN PUT (285, 110), D7(), PSET
IF RED MOD 10 = 8 THEN PUT (285, 110), D8(), PSET
IF RED MOD 10 = 9 THEN PUT (285, 110), D9(), PSET
                                            
IF INT(RED / 10) = 0 THEN PUT (260, 110), D0(), PSET
IF INT(RED / 10) = 1 THEN PUT (260, 110), D1(), PSET
IF INT(RED / 10) = 2 THEN PUT (260, 110), D2(), PSET
IF INT(RED / 10) = 3 THEN PUT (260, 110), D3(), PSET
IF INT(RED / 10) = 4 THEN PUT (260, 110), D4(), PSET
IF INT(RED / 10) = 5 THEN PUT (260, 110), D5(), PSET
IF INT(RED / 10) = 6 THEN PUT (260, 110), D6(), PSET
IF INT(RED / 10) = 7 THEN PUT (260, 110), D7(), PSET
IF INT(RED / 10) = 8 THEN PUT (260, 110), D8(), PSET
IF INT(RED / 10) = 9 THEN PUT (260, 110), D9(), PSET
      
'BLUE
IF BLU MOD 10 = 0 THEN PUT (285, 159), D0(), PSET
IF BLU MOD 10 = 1 THEN PUT (285, 159), D1(), PSET
IF BLU MOD 10 = 2 THEN PUT (285, 159), D2(), PSET
IF BLU MOD 10 = 3 THEN PUT (285, 159), D3(), PSET
IF BLU MOD 10 = 4 THEN PUT (285, 159), D4(), PSET
IF BLU MOD 10 = 5 THEN PUT (285, 159), D5(), PSET
IF BLU MOD 10 = 6 THEN PUT (285, 159), D6(), PSET
IF BLU MOD 10 = 7 THEN PUT (285, 159), D7(), PSET
IF BLU MOD 10 = 8 THEN PUT (285, 159), D8(), PSET
IF BLU MOD 10 = 9 THEN PUT (285, 159), D9(), PSET
                                            
IF INT(BLU / 10) = 0 THEN PUT (260, 159), D0(), PSET
IF INT(BLU / 10) = 1 THEN PUT (260, 159), D1(), PSET
IF INT(BLU / 10) = 2 THEN PUT (260, 159), D2(), PSET
IF INT(BLU / 10) = 3 THEN PUT (260, 159), D3(), PSET
IF INT(BLU / 10) = 4 THEN PUT (260, 159), D4(), PSET
IF INT(BLU / 10) = 5 THEN PUT (260, 159), D5(), PSET
IF INT(BLU / 10) = 6 THEN PUT (260, 159), D6(), PSET
IF INT(BLU / 10) = 7 THEN PUT (260, 159), D7(), PSET
IF INT(BLU / 10) = 8 THEN PUT (260, 159), D8(), PSET
IF INT(BLU / 10) = 9 THEN PUT (260, 159), D9(), PSET
                                           

IF NIL = 0 AND RED > BLU THEN GOSUB REDWIN: GOTO OPTAGAIN
IF NIL = 0 AND RED < BLU THEN GOSUB BLUWIN: GOTO OPTAGAIN
IF NIL = 0 AND RED = 30 THEN GOSUB TIE: GOTO OPTAGAIN

IF BLU = 0 THEN GOSUB REDWIN: GOTO OPTAGAIN
IF RED = 0 THEN GOSUB BLUWIN: GOTO OPTAGAIN

RED = 0: BLU = 0: NIL = 0
RETURN

'----------PROCEDURE TO REDUCE PIECES THAT SHOULD NOT BE BIG------------------
REDUCE:
IF CO = 1 AND P = 1 THEN PUT (X1, Y1), BRP(), XOR: PUT (X1, Y1), RP(): PUT (X1, Y1), CR()
IF CO = 2 AND P = 1 THEN PUT (X1, Y1), BBP(), XOR: PUT (X1, Y1), BP(): PUT (X1, Y1), CR()
RETURN

'------------BEGINNING OF CURSER MOVMENT AND DISPLAY--------------------------
CRU:
IF GB(A, B - 1) <> 4 THEN GOTO CONTU ELSE RETURN
CONTU:
GOSUB KEYOFF
Y = Y - 24
PUT (X, Y), CR()
PUT (X, Y + 24), CR()
B = B - 1
RETURN

CRD:
IF GB(A, B + 1) <> 4 THEN GOTO CONTD ELSE RETURN
CONTD:
GOSUB KEYOFF
Y = Y + 24
PUT (X, Y), CR()
PUT (X, Y - 24), CR()
B = B + 1
RETURN

CRL:
IF GB(A - 1, B) <> 4 THEN GOTO CONTL ELSE RETURN
CONTL:
GOSUB KEYOFF
X = X - 30
PUT (X, Y), CR()
PUT (X + 30, Y), CR()
A = A - 1
RETURN

CRR:
IF GB(A + 1, B) <> 4 THEN GOTO CONTR ELSE RETURN
CONTR:
GOSUB KEYOFF
X = X + 30
PUT (X, Y), CR()
PUT (X - 30, Y), CR()
A = A + 1
RETURN

'----------------------------COMPUTER OPPONENT--------------------------------
COMPUTER:
C = 0: D = 0: C1 = 0: D1 = 0
TAKE = 0: BSTAKE = 0
MOVING = 0
GOSUB ENDGAME
PUT (268, 55), BL(), PSET

COUNT:
POB = 0
FOR PIECES1 = 1 TO 8
    FOR PIECES2 = 1 TO 8
        IF GB(PIECES1, PIECES2) = 2 THEN POB = POB + 1
    NEXT
NEXT
IF POB = 0 THEN GOTO ENDGAME


'ISOLATION CHECK
FOR FIND1 = 1 TO 8
    FOR FIND2 = 1 TO 8
        IF GB(FIND1, FIND2) = 2 THEN GOSUB ISOCHK
        IF ISOLATE = 25 THEN ISOPIECE = ISOPIECE + 1
    NEXT
NEXT
IF ISOPIECE = POB THEN GOTO COMPPASS


'PIECE DETECTION CYCLE
FOR FP1 = 1 TO 8
    FOR FP2 = 1 TO 8
        IF GB(FP1, FP2) = 2 THEN GOSUB FINDMOVE
    NEXT
NEXT
GOTO KEEP


FINDMOVE:

'---MOVE---
FOR OAP1 = -1 TO 1
    FOR OAP2 = -1 TO 1

        IF GB(FP1 + OAP1, FP2 + OAP2) = 0 THEN TAKE = 1: GOSUB CNTMOVE
        IF TAKE > BSTAKE THEN BSTAKE = TAKE: C = (FP1 + OAP1): D = (FP2 + OAP2): C1 = FP1: D1 = FP2

    NEXT
NEXT

'---JUMP---
FOR TAP1 = -2 TO 2
    FOR TAP2 = -2 TO 2

        FOR ELIM1 = -1 TO 1
            FOR ELIM2 = -1 TO 1
                IF TAP1 = ELIM1 AND TAP2 = ELIM2 THEN GOTO SKIP 'ELIMINATE MIDDLE
            NEXT
        NEXT

        IF GB(FP1 + TAP1, FP2 + TAP2) = 0 THEN TAKE = 0: GOSUB CNTJUMP
        IF TAKE > BSTAKE THEN BSTAKE = TAKE: C = (FP1 + TAP1): D = (FP2 + TAP2): C1 = FP1: D1 = FP2

        SKIP:
    NEXT
NEXT

RETURN


'---SUBROUTINES---------------------------------------------------------------

CNTMOVE: 'CHECK HOW MANY TAKEN
TAKE = 1
FOR CHK1 = -1 TO 1
    FOR CHK2 = -1 TO 1
        IF GB(FP1 + OAP1 + CHK1, FP2 + OAP2 + CHK2) = 1 THEN TAKE = TAKE + 1
    NEXT
NEXT
RETURN

CNTJUMP:
TAKE = 0
FOR CHK3 = -1 TO 1
    FOR CHK4 = -1 TO 1
        IF GB(FP1 + TAP1 + CHK3, FP2 + TAP2 + CHK4) = 1 THEN TAKE = TAKE + 1
    NEXT
NEXT
RETURN

ISOCHK:
FOR ISO1 = -2 TO 2
    FOR ISO2 = -2 TO 2
        IF GB(FIND1 + ISO1, FIND2 + ISO2) <> 0 THEN ISOLATE = ISOLATE + 1
    NEXT
NEXT
RETURN

COMPPASS:
PRINT "DSFSDF"
CO = 1
GOTO CURSER


KEEP:
XCD = (C - 1) * 30 + 5: YCD = (D - 1) * 24 + 5
XCD1 = (C1 - 1) * 30 + 5: YCD1 = (D1 - 1) * 24 + 5

IF C = C1 + 2 THEN GOTO COMPJUMP
IF C = C1 - 2 THEN GOTO COMPJUMP
IF D = D1 + 2 THEN GOTO COMPJUMP
IF D = D1 - 2 THEN GOTO COMPJUMP

IF C = C1 + 1 THEN GOTO COMPMAKE
IF C = C1 - 1 THEN GOTO COMPMAKE
IF D = D1 + 1 THEN GOTO COMPMAKE
IF D = D1 - 1 THEN GOTO COMPMAKE

COMPMAKE:
PUT (XCD, YCD), BP()
GB(C, D) = 2
GOSUB MS
GOTO COMPCREATE

COMPJUMP:
PUT (XCD1, YCD1), BP(), XOR
PUT (XCD, YCD), BP()
GB(C, D) = 2
GB(C1, D1) = 0
GOSUB JS
GOTO COMPCREATE

COMPCREATE:
IF GB(C + 1, D + 1) = 1 THEN GB(C + 1, D + 1) = 2: PUT (XCD + 30, YCD + 24), RP(): PUT (XCD + 30, YCD + 24), BP(): GOSUB SS
IF GB(C + 0, D + 1) = 1 THEN GB(C + 0, D + 1) = 2: PUT (XCD + 0, YCD + 24), RP(): PUT (XCD + 0, YCD + 24), BP(): GOSUB SS
IF GB(C - 1, D + 1) = 1 THEN GB(C - 1, D + 1) = 2: PUT (XCD - 30, YCD + 24), RP(): PUT (XCD - 30, YCD + 24), BP(): GOSUB SS
IF GB(C + 1, D + 0) = 1 THEN GB(C + 1, D + 0) = 2: PUT (XCD + 30, YCD + 0), RP(): PUT (XCD + 30, YCD + 0), BP(): GOSUB SS
IF GB(C - 1, D + 0) = 1 THEN GB(C - 1, D + 0) = 2: PUT (XCD - 30, YCD + 0), RP(): PUT (XCD - 30, YCD + 0), BP(): GOSUB SS
IF GB(C + 1, D - 1) = 1 THEN GB(C + 1, D - 1) = 2: PUT (XCD + 30, YCD - 24), RP(): PUT (XCD + 30, YCD - 24), BP(): GOSUB SS
IF GB(C + 0, D - 1) = 1 THEN GB(C + 0, D - 1) = 2: PUT (XCD + 0, YCD - 24), RP(): PUT (XCD + 0, YCD - 24), BP(): GOSUB SS
IF GB(C - 1, D - 1) = 1 THEN GB(C - 1, D - 1) = 2: PUT (XCD - 30, YCD - 24), RP(): PUT (XCD - 30, YCD - 24), BP(): GOSUB SS
IF CO = 1 THEN CO = 2 ELSE CO = 1
GOSUB ENDGAME
PUT (268, 55), RL(), PSET
GOTO CURSER

'------------------BEGINNING OF SOUND GENERATION------------------------------
' #2011: The original sounds didn't sound right and were quite slow on QB64,
' so I've modified them a bit. I've commented out the originals and replaced
' them at the bottom of this file.
'SS:
'IF S$ = "N" OR S$ = "n" THEN RETURN
'FOR U = 500 TO 100 STEP -25
'    SOUND U + U, .1
'    SOUND U, .1
'    SOUND U - U / 2, .1
'NEXT
'RETURN
'
'JS:
'IF S$ = "N" OR S$ = "n" THEN RETURN
'FOR U = 80 TO 405 STEP 25
'    SOUND U - U / 2, .1
'    SOUND U + U, .1
'    SOUND U, .1
'NEXT
'RETURN
'
'MS:
'IF S$ = "N" OR S$ = "n" THEN RETURN
'FOR U = 625 TO 75 STEP -25
'    SOUND U, .1
'    SOUND U - U / 2, .1
'NEXT
'RETURN

KEYOFF:
KEY(11) OFF
KEY(12) OFF
KEY(13) OFF
KEY(14) OFF
RETURN

'------------------GET OPTIONS------------------------------------------------
OPTSOUND:
GET (95, 90)-(160, 115), WDW(): PUT (95, 90), SO(), PSET
BEGIN1:
O$ = INKEY$
IF O$ = "Y" OR O$ = "y" THEN S$ = "Y"
IF O$ = "N" OR O$ = "n" THEN S$ = "N"
IF O$ = "" THEN GOTO BEGIN1
PUT (95, 90), WDW(), PSET
RETURN

OPTPASS:
GOSUB REDUCE
GET (95, 90)-(160, 115), WDW(): PUT (95, 90), PO(), PSET
BEGIN2:
O$ = INKEY$
IF CO = 2 AND O$ = "Y" OR CO = 2 AND O$ = "y" THEN CO = 1: PUT (95, 90), WDW(), PSET: GOTO CURSER
IF CO = 1 AND O$ = "Y" OR CO = 1 AND O$ = "y" THEN CO = 2: PUT (95, 90), WDW(), PSET: GOTO CURSER
IF O$ = "N" OR K$ = "n" THEN PUT (95, 90), WDW(), PSET: RETURN
IF O$ = "" THEN GOTO BEGIN2
PUT (95, 90), WDW(), PSET
RETURN

OPTQUIT:
GET (95, 90)-(160, 115), WDW(): PUT (95, 90), QO(), PSET
BEGIN3:
O$ = INKEY$
IF O$ = "Y" OR O$ = "y" THEN GOTO THEEND
IF O$ = "N" OR O$ = "n" THEN PUT (95, 90), WDW(), PSET: RETURN
IF O$ = "" THEN GOTO BEGIN3

OPTAGAIN:
PUT (95, 90), AO(), PSET
BEGIN4:
O$ = INKEY$
IF O$ = "Y" OR O$ = "y" THEN TT = 1: GOSUB CLRARRAY: GOTO INIT
IF O$ = "N" OR O$ = "n" THEN GOTO THEEND
GOTO BEGIN4

OPTFOR:
GET (95, 90)-(160, 115), WDW(): PUT (95, 90), FO(), PSET
BEGIN5:
O$ = INKEY$

IF CO = 1 AND O$ = "Y" OR CO = 1 AND O$ = "y" THEN
    PUT (95, 90), WDW(), PSET
    GOSUB BLUWIN: GOSUB CLRARRAY: TT = 1: GOTO INIT
END IF

IF CO = 2 AND O$ = "Y" OR CO = 2 AND O$ = "y" THEN
    PUT (95, 90), WDW(), PSET
    GOSUB REDWIN: GOSUB CLRARRAY: TT = 1: GOTO INIT
END IF

IF O$ = "N" OR O$ = "n" THEN PUT (95, 90), WDW(), PSET: GOTO CURSER
IF O$ <> "Y" OR O$ <> "y" OR O$ <> "N" OR O$ <> "n" THEN GOTO BEGIN5
IF O$ = "" THEN GOTO BEGIN5
RETURN

REDWIN:
GET (95, 90)-(160, 115), WDW(): PUT (95, 90), RWIN(), PSET
FOR WW = 1 TO 10000: NEXT
PUT (95, 90), WDW(), PSET
RETURN

BLUWIN:
GET (95, 90)-(160, 115), WDW(): PUT (95, 90), BWIN(), PSET
FOR WW = 1 TO 10000: NEXT
PUT (95, 90), WDW(), PSET
RETURN

TIE:
GET (100, 90)-(160, 118), WDW(): PUT (100, 90), TIE(), PSET
FOR WW = 1 TO 10000: NEXT
PUT (100, 90), WDW(), PSET
RETURN

CLRARRAY:
FOR XYZ1 = 1 TO 8
    FOR XYZ2 = 1 TO 8
        GB(XYZ1, XYZ2) = 0
    NEXT
NEXT
RETURN

TITLE:
'--------------------------TITLE SCREEN AND SONG------------------------------

COLOR 12: LOCATE 12, 13: PRINT "JUST A SECOND."
COLOR 8
LOCATE 1, 16: PRINT "PERPLEX"
LOCATE 3, 7: PRINT "Written by Ray Vanderborght"
LOCATE 5, 8: PRINT "Concept by Matthew Ahola"
LOCATE 20, 4: PRINT "COPYRIGHT 1991 Ray Vanderborght"

' #2011: real delay instead of no-op loop
delay (20)
'FOR WW = 1 TO 10000: NEXT

L1$ = "T150 O0 ACBDCABG P32 CEDFEDEC P32 T140 O1 ECD < B > CD L3 E L4 P7 O0 A < CB > DC < B >  L2 < A > L4 P4"

DEF SEG = VARSEG(TPLEX(1))
BLOAD "TITLE.IMG", VARPTR(TPLEX(1))
CLS

PUT (60, 60), TPLEX()

' Commenting out the music since QB64 can't handle it, and adding short delay
'PLAY "MB X" + VARPTR$(L1$) 'START MUSIC
delay (20)

GOSUB LOADING
PLAY "MF"
CLS
RETURN

LOADING:
DEF SEG = VARSEG(BBP(1))
BLOAD "BBP.IMG", VARPTR(BBP(1))

DEF SEG = VARSEG(BP(1))
BLOAD "MBP.IMG", VARPTR(BP(1))

DEF SEG = VARSEG(SBP(1))
BLOAD "SBP.IMG", VARPTR(SBP(1))

DEF SEG = VARSEG(BRP(1))
BLOAD "BRP.IMG", VARPTR(BRP(1))

DEF SEG = VARSEG(RP(1))
BLOAD "MRP.IMG", VARPTR(RP(1))

DEF SEG = VARSEG(SRP(1))
BLOAD "SRP.IMG", VARPTR(SRP(1))

DEF SEG = VARSEG(BA(1))
BLOAD "PILAR.IMG", VARPTR(BA(1))

DEF SEG = VARSEG(SO(1))
BLOAD "SNDOPT.IMG", VARPTR(SO(1))

DEF SEG = VARSEG(FO(1))
BLOAD "FOROPT.IMG", VARPTR(FO(1))

DEF SEG = VARSEG(AO(1))
BLOAD "AGAINOPT.IMG", VARPTR(AO(1))

DEF SEG = VARSEG(PO(1))
BLOAD "PASSOPT.IMG", VARPTR(PO(1))

DEF SEG = VARSEG(QO(1))
BLOAD "QUITOPT.IMG", VARPTR(QO(1))

DEF SEG = VARSEG(RWIN(1))
BLOAD "REDWON.IMG", VARPTR(RWIN(1))

DEF SEG = VARSEG(BWIN(1))
BLOAD "BLUWON.IMG", VARPTR(BWIN(1))

DEF SEG = VARSEG(Perplex(1))
BLOAD "LOGOPLEX.IMG", VARPTR(Perplex(1))

DEF SEG = VARSEG(RL(1))
BLOAD "REDIND.IMG", VARPTR(RL(1))

DEF SEG = VARSEG(BL(1))
BLOAD "BLUIND.IMG", VARPTR(BL(1))

DEF SEG = VARSEG(D0(1))
BLOAD "DIGIT0.IMG", VARPTR(D0(1))

DEF SEG = VARSEG(D1(1))
BLOAD "DIGIT1.IMG", VARPTR(D1(1))

DEF SEG = VARSEG(D2(1))
BLOAD "DIGIT2.IMG", VARPTR(D2(1))

DEF SEG = VARSEG(D3(1))
BLOAD "DIGIT3.IMG", VARPTR(D3(1))

DEF SEG = VARSEG(D4(1))
BLOAD "DIGIT4.IMG", VARPTR(D4(1))

DEF SEG = VARSEG(D5(1))
BLOAD "DIGIT5.IMG", VARPTR(D5(1))

DEF SEG = VARSEG(D6(1))
BLOAD "DIGIT6.IMG", VARPTR(D6(1))

DEF SEG = VARSEG(D7(1))
BLOAD "DIGIT7.IMG", VARPTR(D7(1))

DEF SEG = VARSEG(D8(1))
BLOAD "DIGIT8.IMG", VARPTR(D8(1))

DEF SEG = VARSEG(D9(1))
BLOAD "DIGIT9.IMG", VARPTR(D9(1))

DEF SEG = VARSEG(BBOX(1))
BLOAD "BLUBOX.IMG", VARPTR(BBOX(1))

DEF SEG = VARSEG(RBOX(1))
BLOAD "REDBOX.IMG", VARPTR(RBOX(1))

DEF SEG = VARSEG(CR(1))
BLOAD "CURSER.IMG", VARPTR(CR(1))

DEF SEG = VARSEG(TIE(1))
BLOAD "TIE.IMG", VARPTR(TIE(1))

DEF SEG = VARSEG(PLYR(1))
BLOAD "PLAYERS.IMG", VARPTR(PLYR(1))

DEF SEG

RETURN

THEEND:
CLS
COLOR 9
PRINT
PRINT "       THANKS FOR PLAYING PERPLEX"
PRINT
PRINT "        This game is shareware."
PRINT "   Give copies to friends and BBS's"
PRINT
PRINT "    PRESS ANY KEY TO RETURN TO DOS"

WFKP:
K$ = INKEY$
IF K$ = "" THEN GOTO WFKP ELSE END

' #2011: redefining sounds to sound right on qb64
SS:
IF S$ = "N" OR S$ = "n" THEN RETURN
FOR U = 500 TO 100 STEP -50
    SOUND U, .05
NEXT
RETURN

JS:
IF S$ = "N" OR S$ = "n" THEN RETURN
FOR U = 100 TO 450 STEP 50
    SOUND U, .05
NEXT
RETURN

MS:
IF S$ = "N" OR S$ = "n" THEN RETURN
FOR U = 600 TO 100 STEP -50
    SOUND U, .05
    SOUND U - U / 2, .05
NEXT
RETURN

' #2011: Adding real delay for modern hardware
' http://www.tek-tips.com/viewthread.cfm?qid=91735&page=31
SUB delay (numTicks%)
FOR i% = 1 TO numTicks%
    st# = TIMER
    WHILE TIMER = st#: WEND 'TIMER will change 1/18.2 of a second after the previous line
NEXT i%
END SUB

