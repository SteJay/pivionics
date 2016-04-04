import pygame
import pygame.gfxdraw
import sys
import random
import socket
import json
from time import time
def draw(mylist):
    pygame.init()
    screen=pygame.display.set_mode((600,600),pygame.DOUBLEBUF|pygame.HWSURFACE,16)
    clock = pygame.time.Clock()
    quitme=0

    s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("127.0.0.1",5000))
    s.listen(1)

    conn,addr=s.accept()
    l=[]
    msgq=""
    fcount=0
    ftime=0
    etime=0
    dtime=0
    ptime=0
    rtime=0
    while quitme<1:
        sof=time()
        fcount+=1
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                quitme= 1
            elif event.type == pygame.KEYUP:
                if event.key == pygame.K_ESCAPE:
                    quitme= 2
        etime+=time()-sof; timer=time()
        data=conn.recv(1024)
        if data:
            msgq+=data
        dtime+=time()-timer; timer=time()
        f=msgq.find("}")
        while f>0:
            if f>-1:
                f+=1
                #z=eval(msgq[:f])
                z=json.loads(msgq[:f])
                l.append(z)
                msgq=msgq[f:]
            f=msgq.find("}")
        ptime+=time()-timer; timer=time()

        if not data:
            for x in l:
                actual_draw(x,screen)
            #clock.tick(100)
        pygame.display.flip()
        rtime+=time()-timer;
        ftime+=time()-sof
        if fcount >= 100:
            print "Frm:{:01.4f} Evt:{:01.4f} Dta:{:01.4f} Prs:{:01.4f} dRw:{:01.4f} for {} frms, {}fps".format(ftime, etime, dtime, ptime,rtime,fcount, 1/(ftime/fcount))
            fcount=0
            ftime=0
            etime=0
            dtime=0
            ptime=0
            rtime=0
        
    s.close()
    pygame.quit()

def actual_draw(mylist,screen):
    points=[]
    mode="line"
    for z in mylist.keys():
        if z=="mode":
            mode=mylist[z]
        elif z=="points":
            last=[]
            for p in mylist[z]:
                points.append((p[0]+300,p[1]+300))
                if not last:
                    last=points
                if mode=="triangle" and len(points) > 0:
                    #pygame.draw.polygon(screen,(255,0,0),[
                    #    (points[0][0],points[0][1]),
                    #    (points[1][0],points[1][1]),
                    #    (points[2][0],points[2][1]),
                    #    ],0)
                    pygame.gfxdraw.filled_trigon(
                        screen,
                        points[0][0],points[0][1],
                        points[1][0],points[1][1],
                        points[2][0],points[2][1],
                        (random.random()*255,random.random()*255,random.random()*255)
                    )
                    points=[]
                elif mode=="line" and len(points) > 0:
                    #pygame.draw.line(screen,(255,0,0),(points[0][0],points[0][1]),(points[1][0],points[1][1]),1)
                    pygame.gfxdraw.line(
                        screen,
                        points[0][0],points[0][1],
                        points[1][0],points[1][1],
                        (random.random()*255,random.random()*255,random.random()*255)
                    )
                    points=[]


if __name__=="__main__":
    #inp = sys.stdin.readlines()
    inp=""
    draw(inp)    
