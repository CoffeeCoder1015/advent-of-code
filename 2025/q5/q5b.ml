let rec load_intervals chan = 
  let parse_line range = 
    let strl = String.split_on_char '-' range in
    let a = String.trim ( List.nth strl 0 ) in
    let b = String.trim ( List.nth strl 1 ) in
    (int_of_string a, int_of_string b)
  in
  let rec line_consumer list = 
    let line = input_line chan in
    let empty = String.trim line = "" in
    if not empty then 
      line_consumer (list @ [parse_line line])
    else
      list
  in
  List.sort compare ( line_consumer [] )
;;

let in_chan = open_in "q5.txt" in 
  let intervals = load_intervals in_chan in
  let rec proc_intervals ans min max remaining = 
    if ( List.length remaining  ) = 0 then
      ans + (max-min) + 1
    else
    let lower,upper = List.hd remaining in
    let new_remaining = List.tl remaining in
    if max < lower then
      proc_intervals (ans + (max-min)+1) lower upper new_remaining
    else
      if upper > max then
        proc_intervals ans min upper new_remaining
      else
        proc_intervals ans min max new_remaining
      in
    let lower, upper = List.hd intervals in
    let tail = List.tl intervals in
    let ans = proc_intervals 0 lower upper tail in
    Printf.printf "%d\n" ans
;;