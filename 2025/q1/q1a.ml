let line_parser line_str = 
  let delta = String.sub line_str 1 (String.length line_str - 2) in
  match String.get line_str 0 with
  | 'L' -> -( int_of_string delta  )
  | 'R' -> int_of_string delta
  | _ -> assert false
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
      let delta = line_parser line in
      let new_ptr = mod_euclid (pointer + delta) 100 in
      line_consumer new_ptr ( counter +  int_of_bool ( new_ptr = 0 ))
    | exception End_of_file -> 
      close_in in_chan;
      counter
    in
  Printf.printf "%d\n" ( line_consumer 50 0  )
;; 

