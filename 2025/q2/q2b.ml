let split_range range = 
  let list = String.split_on_char '-' range in 
  list
;;

let prime_factor n = 
  let module S = Set.Make(Int)  in
  let rec aux d n prime_set = 
    if n = 1 then S.elements prime_set
    else if n mod d = 0 then aux d (n/d) (S.add d prime_set)
    else if d*d > n then S.elements (S.add n prime_set)
    else aux (d+1) n prime_set
  in
  aux 2 n S.empty
  ;;

let solve ans range = 
  let splited = split_range range in 
  let first = List.nth splited 0 in
  let second = List.nth splited 1 in
  let a = int_of_string first  in
  let n = String.length first in
  let b = int_of_string second in
  let m = String.length second in
    match ( n,m ) with
    | (n,m) when n = m -> 
        let factors =  prime_factor n in
        List.iter (Printf.printf "%d ") factors;
        Printf.printf "\n";
        ans
    | (n,m) when n < m -> 
        ans
    | _ -> assert false 
;;

(* reads in entire file *)
let in_chan = open_in "q2.txt" in 
let len = in_channel_length in_chan in
let content = really_input_string in_chan len in

(* Splits content string by , and prints everything line by line *)
let parts = String.split_on_char ',' content in
let ans = List.fold_left solve 0 parts in
print_endline ( string_of_int ans )