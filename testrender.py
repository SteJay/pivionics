import pygame
import pygame.gfxdraw
import sys
import random
import socket

def draw(mylist):
    pygame.init()
    screen=pygame.display.set_mode((1280,720),pygame.DOUBLEBUF,24)
    clock = pygame.time.Clock()
    quitme=0

    s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("127.0.0.1",5000))
    s.listen(1)

    conn,addr=s.accept()
    l=[]
    msgq=""
    while quitme<1:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                quitme= 1
            elif event.type == pygame.KEYUP:
                if event.key == pygame.K_ESCAPE:
                    quitme= 2
        data=conn.recv()
        if data:
            msgq+=data
        f=msgq.find("}")
        if f>-1:
            f+=1
            z=eval(msgq[:f])
            l.append(z)
            msgq=msgq[f:]
        else:
            for x in l:
                actual_draw(x,screen)
            pygame.display.flip()
            clock.tick(40)
    s.close()
    pygame.quit()

def actual_draw(mylist,screen):
    points=[]
    mode="line"
    for z in mylist.keys():
        if z=="mode":
            mode=mylist[z]
        elif z=="points":
            for p in mylist[z]:
                points.append((p[0]+360,p[1]+360))
                if mode=="triangle" and len(points) > 2:
                    #pygame.draw.polygon(screen,(255,0,0),points,0)
                    pygame.gfxdraw.filled_trigon(
                        screen,
                        points[0][0],points[0][1],
                        points[1][0],points[1][1],
                        points[2][0],points[2][1],
                        (random.random()*255,random.random()*255,random.random()*255)
                    )
                    points=[]
                elif mode=="line" and len(points) > 1:
                    #pygame.draw.polygon(screen,(255,0,0),points,0)
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
