let parse_lines line = 
  let raw_split = String.split_on_char ' ' line in
  let filter = List.filter (fun x -> String.length x != 0 ) raw_split in
  filter
;;

let to_int_list str_list =  
  List.map int_of_string str_list
;;

let in_chan = open_in "test.txt" in
  let length = in_channel_length in_chan in
  let lines = List.map parse_lines ( String.split_on_char '\n' ( really_input_string in_chan length ) ) in  
  let n = List.length lines in
  let operators = List.nth lines (n-1) in 
  let rec vec_op acc a b op =
    if List.is_empty a then
      acc
    else
    let op_mapped = match List.hd op with
    | "*" -> Int.mul
    | "+" -> Int.add 
    | _ -> assert false
    in
    let new_acc = acc @ [op_mapped (List.hd a) (List.hd b)] in
    vec_op new_acc (List.tl a) (List.tl b) (List.tl op)
  in
  let rec helper i acc remaining = 
    if i = ( n-2 ) then
      List.fold_left (fun a x -> a+x) 0 acc 
    else
      let new_acc = vec_op [] acc (to_int_list ( List.hd remaining )) operators in
      helper ( i+1 ) new_acc (List.tl remaining)
    in 
  let result = helper 0 (to_int_list ( List.hd lines )) (List.tl lines) in
  Printf.printf "%d\n" result
;;
