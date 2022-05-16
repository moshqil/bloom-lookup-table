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
f2 = open("14chr_first.txt", 'w')
f3 = open("14chr_second.txt", 'w')
for line in f1:
    a = line.split()
    if a[3] != "0|0":
        f2.write(a[0] + ' ' + a[1] + ' ' + a[2] + ' ' + a[3] + '\n')
    if a[4] != "0|0":
        f3.write(a[0] + ' ' + a[1] + ' ' + a[2] + ' ' + a[4] + '\n')


