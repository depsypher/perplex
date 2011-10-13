'Anything that refers to:
'?/FRAMEWORKS/one-of-our-frameworks
'Where ? can be:
'"@EXECUTABLE_PATH/.."
'"@LOADER_PATH"
'"/SYSTEM/LIBRARY"
'Needs to be turned into:
'         ./common/one-of-our-frameworks
'And the whole string needs to be left justified within its
'block and null terminated

WIDTH 125, 40

ON ERROR GOTO 2
nf = 0
REDIM f$(1)
OPEN "fwhack.txt" FOR INPUT AS #1

DO UNTIL EOF(1)

    LINE INPUT #1, a$
    a$ = LTRIM$(RTRIM$(a$))
    IF LEN(a$) THEN
        IF RIGHT$(a$, 1) = ":" THEN
            d$ = LEFT$(a$, LEN(a$) - 1)
            d$ = RIGHT$(d$, LEN(d$) - 2)
            PRINT "Directory:[" + d$ + "]"
            GOTO done
        END IF

        1
        FOR x = 1 TO LEN(a$)
            c = ASC(a$, x)
            IF c = 32 OR c = 9 THEN
                nf = nf + 1
                REDIM _PRESERVE f$(nf)
                f$(nf) = d$ + "/" + LEFT$(a$, x - 1)
                PRINT "File:[" + f$(nf) + "]"
                a$ = LTRIM$(RIGHT$(a$, LEN(a$) - x))
                GOTO 1
            END IF
        NEXT
        nf = nf + 1
        REDIM _PRESERVE f$(nf)
        f$(nf) = d$ + "/" + a$
        PRINT "File:[" + f$(nf) + "]"

    END IF
    done:
LOOP
CLOSE #1
PRINT "Files:"; nf
FOR i = 1 TO nf

    e = 0
    OPEN "common/" + f$(i) FOR BINARY AS #1
    IF e = 0 THEN
        ca$ = SPACE$(LOF(1))
        GET #1, , ca$

        a$ = UCASE$(ca$)
        fl = 0
        z1 = 1
        3
        z = INSTR(z1, a$, "FRAMEWORKS")
        IF z THEN
            IF fl = 0 THEN PRINT "In File:[" + f$(i) + "]": fl = 1
            x1 = z - 30
            x2 = z + 90
            IF x1 < 1 THEN x1 = 1
            IF x2 > LEN(a$) THEN x2 = LEN(a$)
            FOR x = x1 TO x2
                a = ASC(a$, x)
                IF a = 0 THEN
                    PRINT CHR$(254);
                ELSE
                    IF a < 32 THEN PRINT "?"; ELSE PRINT CHR$(a);
                END IF
            NEXT: PRINT


            'Anything that refers to:
            '?/FRAMEWORKS/one-of-our-frameworks
            'Where ? can be:
            '"@EXECUTABLE_PATH/.."
            '"@LOADER_PATH"
            '"/SYSTEM/LIBRARY"
            'Needs to be turned into:
            '         ./common/one-of-our-frameworks
            'And the whole string needs to be left justified within its
            'block and null terminated
            'Careful of CASE!

            'Does one of our frameworks follow?
            ourfw = 0
            FOR fw = 1 TO 7
                IF fw = 1 THEN fw$ = "SDL.FRAMEWORK"
                IF fw = 2 THEN fw$ = "SDL_IMAGE.FRAMEWORK"
                IF fw = 3 THEN fw$ = "SDL_MIXER.FRAMEWORK"
                IF fw = 4 THEN fw$ = "SDL_TTF.FRAMEWORK"
                IF fw = 5 THEN fw$ = "SDL_NET.FRAMEWORK"
                IF fw = 6 THEN fw$ = "MIKMOD.FRAMEWORK"
                IF fw = 7 THEN fw$ = "SMPEG.FRAMEWORK"
                IF MID$(a$, z + 11, LEN(fw$)) = fw$ THEN
                    ourfw = 1
                    EXIT FOR
                END IF
            NEXT
            IF ourfw THEN
                'Is it prepended by a valid header string
                gothed = 0
                FOR h = 1 TO 3
                    IF h = 1 THEN h$ = "@EXECUTABLE_PATH/.."
                    IF h = 2 THEN h$ = "@LOADER_PATH"
                    IF h = 3 THEN h$ = "/SYSTEM/LIBRARY"
                    h$ = CHR$(0) + h$ + "/"
                    IF MID$(a$, z - LEN(h$), LEN(h$)) = h$ THEN
                        gothed = 1
                        EXIT FOR
                    END IF
                NEXT
                IF gothed THEN
                    'Insert "./common/"
                    z2 = z - LEN(h$) + 1
                    newh$ = "./common/"
                    MID$(ca$, z2, LEN(newh$)) = newh$
                    'Find NULL terminator
                    FOR x2 = z TO LEN(a$)
                        IF ASC(a$, x2) = 0 THEN EXIT FOR
                    NEXT
                    IF x2 > LEN(a$) THEN PRINT "Cannot locate NULL terminator!"
                    'Get existing content
                    content$ = MID$(a$, z + 11, x2 - (z + 11))
                    'Move content to new position
                    MID$(ca$, z2 + LEN(newh$), LEN(content$)) = content$
                    'Null remaining content
                    FOR x = z2 + LEN(newh$) + LEN(content$) TO x2
                        ASC(ca$, x) = 0
                    NEXT
                    'Report new content
                    x1 = z - 30
                    x2 = z + 90
                    IF x1 < 1 THEN x1 = 1
                    IF x2 > LEN(ca$) THEN x2 = LEN(a$)
                    FOR x = x1 TO x2
                        a = ASC(ca$, x)
                        IF a = 0 THEN
                            PRINT CHR$(254);
                        ELSE
                            IF a < 32 THEN PRINT "?"; ELSE PRINT CHR$(a);
                        END IF
                    NEXT: PRINT
                    'Left justify existing content
                END IF
            END IF
            z1 = z + 1: GOTO 3
        END IF

        'Commit changes!!!
        PUT #1, 1, ca$

        CLOSE #1
    END IF


NEXT


END
2
e = 1
RESUME NEXT
