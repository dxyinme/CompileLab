

if(__name__ == "__main__"):
    F = open("end.txt","r");
    S = [];
    g = 270 ;
    for i in F.readlines():
        if(i[0] == '#' or i.strip() == ""): pass;
        else:
            S.append(i.strip() + " " + str(g));
            g += 1;
    F.close();
    F = open("reend.txt","w");
    for i in S:
        F.write(i + "\n");
    F.close();