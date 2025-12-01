with open("q1.txt","r") as fio:
    instructions_raw = fio.read()
    
instructions  = instructions_raw.split("\n")

counter  = 0
pointer = 50
for i in instructions:
    old = pointer
    match i[0]:
        case "L":
            pointer-=int(i[1:])
        case "R":
            pointer+=int(i[1:])
    pointer %= 100
    counter += int(pointer == 0)
print(counter)