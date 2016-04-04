import pygame
import pygame.gfxdraw
import sys
import random

def draw(mylist):
    pygame.init()
    screen=pygame.display.set_mode((1280,720),pygame.DOUBLEBUF,24)
    clock = pygame.time.Clock()
    quitme=0
    while quitme<1:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                quitme= 1
            elif event.type == pygame.KEYUP:
                if event.key == pygame.K_ESCAPE:
                    quitme= 2
        points=[]
        for p in mylist:
            points.append((p[0]+360,p[1]+360))
            if len(points) > 2:
                #pygame.draw.polygon(screen,(255,0,0),points,0)
                pygame.gfxdraw.filled_trigon(
                    screen,
                    points[0][0],points[0][1],
                    points[1][0],points[1][1],
                    points[2][0],points[2][1],
                    (random.random()*255,random.random()*255,random.random()*255)
                )
                points=[]
        pygame.display.flip()
        clock.tick(40)
    pygame.quit()
if __name__=="__main__":
    inp = sys.stdin.readlines()
    l=[]
    for i in inp:
        l += eval(i)
    draw(l)    
