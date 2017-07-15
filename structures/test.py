def write_some_data() :

    f = open("testData.csv", "w")

    for i in range(200):
        for j in range(200):
            for z in range(5):
                if z == 0 or z == 1:
                    f.write("a")
                if z == 2 or z == 3 or z == 4:
                    arr = ["2"] * 30
                    f.write("*".join(arr))
                if z != 4:
                    f.write(",")
            f.write(";")
        f.write("\n")

write_some_data()
