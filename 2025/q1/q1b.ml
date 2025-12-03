let line_parser line_str = 
  let line = String.trim line_str in
  let mag = String.sub line 1 (String.length line - 1) in
  let dir = match String.get line_str 0 with
    | 'L' -> -1
    | 'R' -> 1
    | _ -> assert false 
  in
  (dir, int_of_string mag)
;;

let int_of_bool = function 
  | true -> 1
  | false -> 0
;;

let mod_euclid a n =
  let r = a mod n in
  if r < 0 then r + n else r
;;

let in_chan = open_in "q1.txt" in
let rec line_consumer pointer counter =
   match input_line in_chan with
    | line -> 
      let (dir,mag) = line_parser line in
      let complete_cycles = mag/100 in 
      let mag_remainder = mag mod 100 in
      let raw_next_ptr = pointer + dir * mag_remainder in
      let remainder_passes_0 = raw_next_ptr > 99 || (raw_next_ptr <= 0 && pointer != 0) in
      let next_ptr = mod_euclid raw_next_ptr 100 in
      let next_counter = counter + complete_cycles + int_of_bool remainder_passes_0 in
      line_consumer next_ptr next_counter;
    | exception End_of_file -> 
      close_in in_chan;
      counter
    in
  Printf.printf "%d\n" ( line_consumer 50 0  )
;; 

