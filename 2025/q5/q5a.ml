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

let load_IDs chan = 
  let rec line_consumer ids = 
    match input_line chan with
    | line -> 
        let item = int_of_string ( String.trim line ) in
        line_consumer (ids @ [item])
    | exception End_of_file ->
        close_in chan;
        ids
  in
  List.sort compare ( line_consumer [] )
;;

let int_of_bool = function 
  | true -> 1
  | false -> 0
;;

let rec count_fresh ans intervals ids = 
  if List.length ids = 0 || List.length intervals = 0 then
    ans
  else
  let lower,upper = List.nth intervals 0 in
  let latest = List.nth ids 0 in
  if latest > upper then
    count_fresh ans (List.tl intervals) ids 
  else
    let new_ans = ans + int_of_bool (lower <= latest && latest <= upper) in
    count_fresh new_ans intervals (List.tl ids)
;;

let in_chan = open_in "q5.txt" in 
  let intervals = load_intervals in_chan in
  let ids = load_IDs in_chan in
  Printf.printf "%d\n" ( count_fresh 0 intervals ids )
;;