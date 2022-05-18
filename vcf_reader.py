def read_gatk_vcf(path):
    header,data = [],[] #init variables
    i = 0
    with open(path,'r') as f:
        
        for line in f:
            i += 1

            #if i > 1000:
            #    break
            if line.startswith('#'):
                header.append(line.replace('\n','').split('\t'))
            else:
                #data.append(line.replace('\n','').split('\t'))
                a = line.split('\t')
                data.append((a[1], a[3], a[4], a[9], a[10], a[11], a[12]))
                #print(a[1], a[3], a[4], a[9], a[10])
    return header,data

#header, data = read_gatk_vcf("data/1_chr14.vcf")
#with open("result.txt", 'w') as f:
#    for elem in data:
#        f.write(" ".join(map(str, elem)) + '\n')
f1 = open("14chr_4genomes.txt", 'r') 
f2 = open("14chr_first.txt", 'r')
f3 = open("14chr_second.txt", 'r')
f4 = open("14chr_f.txt", 'w')
f5 = open("14chr_s.txt", 'w')
for line in f2:
    a = line.split()
    f4.write(a[0] + ' ' + a[1] + ' ' + a[2] + '\n')
    if a[3] == "0|1":
        f4.write('1\n')
    if a[3] == "1|0":
        f4.write('2\n')
    if a[3] == "1|1":
        f4.write('3\n')
for line in f3:
    a = line.split()
    f5.write(a[0] + ' ' + a[1] + ' ' + a[2] + '\n')
    if a[3] == "0|1":
        f5.write('1\n')
    if a[3] == "1|0":
        f5.write('2\n')
    if a[3] == "1|1":
        f5.write('3\n')



