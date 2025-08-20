#!/usr/bin/env python3
import random

def card_pos(pos, a, b, c, d): # f(x) = ax³ + bx² + cx + d se card ki pos^n ka value nikalna hai
    x3 = pos ** 3
    x2 = pos ** 2
    r = (a * x3 + b * x2 + c * pos + d) % 52
    return r

def c2s(card_num): # card num ko string me convert 
    ikkadoteenchaarpaanchchehsaataathnaudasgulaambegambadshah = ['A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K']
    dilpaanchidieent = ['♠', '♥', '♦', '♣']
    
    dilpaanchidieent_pos = card_num // 13
    ikkadoteenchaarpaanchchehsaataathnaudasgulaambegambadshah_pos = card_num % 13
    
    return f"{ikkadoteenchaarpaanchchehsaataathnaudasgulaambegambadshah[ikkadoteenchaarpaanchchehsaataathnaudasgulaambegambadshah_pos]}{dilpaanchidieent[dilpaanchidieent_pos]}"

def main():
    a = random.randint(1, 6)
    b = random.randint(1, 8)
    c = random.randint(1, 12)
    d = random.randint(0, 51)
    
    print("Welcome to the hideout of the greatest magician ever, Mr. Cubic Poly!")
    print("Some of his dark magic involves his ability, to predict any card at any position.")
    print("But can you predict his cards? He uses a very dark formula...")
    print()
    print("Let the game begin. I will show you the first 10 cards, and your task? ")
    print("Predict the next 100 cards and attain a piece of the secret i hold.")
    print()

    print("Here are the first 10 cards in his sequence:")
    for i in range(10):
        card_num = card_pos(i, a, b, c, d)
        card_str = c2s(card_num)
        print(f"Position {i}: {card_str} (Number: {card_num})")
    
    print()
    print("Now, predict the next 100 cards in his sequence!")
    print("Send them as numbers seperated by spaces (0-51), in a single line:")
    print()

    try:
        uinp = input("> ").strip()
        valuechosen = uinp.split()
        if len(valuechosen) != 100:
            print("galat baat")
            return
        
        try:
            cardchosen = [int(x) for x in valuechosen]
        except ValueError:
            print("galat baat")
            return
        
        sahibaat = True
        for i in range(100):
            satya = card_pos(i + 10, a, b, c, d)
            if cardchosen[i] != satya:
                sahibaat = False
                break
        
        if sahibaat:
            print()
            print("WOAH! You've broken his dark magic succesfully, congrats!")
            with open("flag.txt", "r") as f:
                flag = f.read().strip()
            print(f"Here's Mr. Cubic Poly's secret 🔮: {flag}")
        else:
            print("HAHAHAHA, WRONG Prediction! His dark magic is too strong for you!")
    
    except KeyboardInterrupt:
        print("\ngalat baat")
    except Exception as e:
        print(f"dikkat: {e}")

if __name__ == "__main__":
    main()