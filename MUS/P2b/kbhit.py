# kbhit.py: sencillo módulo para lectura no bloqueante de caracteres simples y teclase de cursor
import pygame
class KBHit:
    def __init__(self) -> None:
        pygame.init()
        pygame.key.set_repeat(100) # repeticion de pulsación
        pygame.display.set_mode((300, 200)) # ventana de pygame
        pygame.display.set_caption('Keyboard input control')
        for event in pygame.event.get(): pass # limpiamos posible input previo

    def getKey(self):
        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN: # si se pulsa tecla
                if event.key == pygame.K_UP: # cursores
                    return 'up'
                elif event.key == pygame.K_DOWN:
                    return 'down'
                elif event.key == pygame.K_LEFT:
                    return 'left'
                elif event.key == pygame.K_RIGHT:
                    return 'right'
                elif event.key == pygame.K_ESCAPE: # escape
                    return 'escape'
                elif event.key == pygame.K_SPACE: # esapcio
                    return 'space'
                else:
                    return event.unicode # letras
            return ''

    def quit(self):
        pygame.quit()