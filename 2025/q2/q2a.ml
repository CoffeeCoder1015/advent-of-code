let split_range range = 
  let list = String.split_on_char '-' range in 
  list
;;

let split_in_half string =
  let length = String.length string in
  let first = String.sub string 0 ( length/2 ) in
  let second = String.sub string ( length/2 ) ( length/ 2 ) in
  first,second
;;


let rec check ans current end_ = 
  if current > end_ then
    ans
  else(
    let str_form = string_of_int current in 
    let n = String.length str_form in
    let pad = n mod 2 in 
      match pad with
        | 0 -> 
            let first,second = split_in_half str_form in
            if first = second then
              check (ans + current) (current + 1) end_
            else
              check ans (current + 1) end_
        | 1 -> check ans (current + 1) end_
        | _ -> assert false
  )
;;


let solve ans range = 
  let splited = split_range range in 
  let first_no = int_of_string (List.nth splited 0) in
  let second_no = int_of_string (List.nth splited 1) in
  let partial_ans = check 0 first_no second_no in
  ans+partial_ans
;;

(* reads in entire file *)
let in_chan = open_in "q2.txt" in 
let len = in_channel_length in_chan in
let content = really_input_string in_chan len in

(* Splits content string by , and prints everything line by line *)
let parts = String.split_on_char ',' content in
let ans = List.fold_left solve 0 parts in
print_endline ( string_of_int ans )