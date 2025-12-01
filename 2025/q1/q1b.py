with open("q1.txt","r") as fio:
    instructions_raw = fio.read()
    
instructions  = instructions_raw.split("\n")

counter  = 0
pointer = 50
for i in instructions:
    match i[0]:
        case "L":
            direct = -1
        case "R":
            direct = 1
    mag = int(i[1:])
    k = direct*mag

    if k >= 0:
        while k > 0:
            delta = 100 - pointer
            increment = min(k,delta)
            k -= increment
            pointer += increment 
            if pointer == 100:
                pointer = 0
            counter += int(pointer == 0)
    else: 
        while k < 0:
            delta = -pointer
            if delta == 0:
                delta = -100
            decrement = max(k,delta)
            k -= decrement
            pointer += decrement
            if pointer < 0:
                pointer += 100
            counter += int(pointer == 0)
    print(i,counter)

print(counter)
