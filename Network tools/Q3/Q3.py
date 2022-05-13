'''
    Q3 code 
    JEFFIN BIJU 
    EE19B085
'''

#convert binary number to decimal
def bin_to_decimal(binary):
    decimal, i, n = 0, 0, 0
    while(binary != 0):
        dec = binary % 10
        decimal = decimal + dec * pow(2, i)
        binary = binary//10
        i += 1
    return decimal

#convert numner from hexadecimal to binary
def hex_to_bin(hexcode):
    res = ''
    for c in hexcode:
        res += "{0:04b}".format(int(c, 16))
    return res

#function to give position of error. Returns 0 if no error
def detectError(arr, r):
    n = len(arr)
    res = 0
    for i in range(r):
        val = 0
        for j in range(n):
            if(j+1 & (2**i) == (2**i)):
                val = val ^ int(arr[j])
        res = res + val*(10**i)
    return int(str(res), 2)

# function to remove checkbits    
def remove_checkbits(string):
    res = ''
    j = 0
    for i in range(len(string)):
        if (i+1)==2**j:
            j += 1
        else:
            res += string[i]
    return res

#function to recover original string
def recover_original_string(code):
    res = ''
    for i in range(len(code)//10):
        hexcode = code[i*10:i*10+10]
        bincode = hex_to_bin(hexcode)
        bincode = bincode[1:-1]
        pos = detectError(bincode, 6)
        if pos!=0:
            bincode = bincode[0:pos-1] + str(1^int(bincode[pos-1])) + bincode[pos:38]
        recovered_bincode = remove_checkbits(bincode)
        for j in range(4):
            res += chr(bin_to_decimal(int(recovered_bincode[j*8:(j+1)*8])))
    return res

code = '044B5281EE2E8BCC8942220109C9D2463BA1D0D0061BBDB1486A839085726203A5B8E044B31D89E44F2B05C9760A6101855E2F2181D1504EA981ADD80EFF0DAD660A03D995E44E2901DDE82F1325AFD206D39C81E83EC3A5C9E8662B97B85C'
print(recover_original_string(code))