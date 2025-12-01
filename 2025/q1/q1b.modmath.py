# i dont like that i havent found the modular arithmetic solution yet
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

    counter += mag//100  # complete loops around the cycle
    mag %= 100 # remainder (= 0 if mag is a factor of 100 thus everything circles back to where it started)

    # perform same thing as `part a` but with remainder
    # but this time it just had to be `out of bounds` or ==0 to increment the counter

    k = direct*mag
    start = pointer
    pointer += k
    if pointer > 99: # it is greater than 99 => it has crossed 0 one last time
        counter += 1
    elif pointer <= 0 and start != 0: # if it is less than equal 0 => it has crossed 0 one last time 
        counter += 1                  # NOTE: if it starts on 0 it cannot cross 0

    pointer %= 100

print(counter)
