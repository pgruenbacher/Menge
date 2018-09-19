rows = 20
columns = 6

startx = 9
starty = 6
spacing = 0.5

border = 16

with open("bigSquare.txt", 'w') as f:
    f.write('%d\n' % border)

    num = 0
    for row in range(rows):
        for column in range(columns):
            x = startx + (spacing * column)
            y = starty - (spacing * row)

            if num > border:
                priori = 0
            else:
                priori = 1

            f.write('%f %f %d\n' % (x, y, priori))
