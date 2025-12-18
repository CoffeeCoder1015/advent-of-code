let vindex str_lst idx = 
  List.map (fun x -> String.get x idx ) str_lst
;;

let in_chan = open_in "q6.txt" in
  let length = in_channel_length in_chan in
  let lines = String.split_on_char '\n' ( really_input_string in_chan length ) in
  let n = List.length lines in
  let numbers = List.filteri (fun i _ -> i != n-1) lines in
  let operators = List.nth lines ( n-1 ) in
  let m = String.length operators in
  let rec helper i acc act op =
    if i = m then
      acc+act
    else
    let current_slice = vindex numbers i in
    let joined = String.trim ( String.of_seq (List.to_seq current_slice) ) in
    if joined = "" then
      helper (i+1) acc act operators.[i]
    else if operators.[i] != ' ' then
      let new_act = int_of_string joined in
      helper (i+1) (acc+act) new_act operators.[i]
    else
      let op_mapped = match op with
      | '*' -> Int.mul
      | '+' -> Int.add 
      | _ -> assert false
      in
      let num = int_of_string joined in
      helper (i+1) acc (op_mapped act num) op 
    in
  let ans = helper 0 0 0 ' ' in
  Printf.printf "%d\n" ans
