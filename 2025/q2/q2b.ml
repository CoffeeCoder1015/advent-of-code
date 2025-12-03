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


  
  
  
let mask total uint_rep = 
  let unit_length = total/uint_rep in
  let const = 10. ** float_of_int unit_length in
  let rec aux i ans = 
    if i >= uint_rep then
      ans
    else
      let rep_const = const ** (float_of_int i) in
      aux (i+1) (ans+(int_of_float rep_const))
    in
    aux 0 0
;;


let eq_seg_sum first second n factor = 
  let a_ = int_of_string first in 
  let b_ = int_of_string second in
  None
;;

let solve ans range = 
  let splited = split_range range in 
  let first = List.nth splited 0 in
  let second = List.nth splited 1 in
  let n = String.length first in
  let m = String.length second in
    match ( n,m ) with
    | (n,m) when n = m -> 
        Printf.printf "Equal: ";
        let factors =  prime_factor n in
        List.iter (Printf.printf "%d ") factors;
        Printf.printf "\n";
        let print_factor f = 
        Printf.printf "Mask:%d\n" ( mask n f ) in
        List.iter print_factor factors;
        eq_seg_sum first second n (List.nth factors 0);
        ans
    | (n,m) when n < m -> 
        Printf.printf "Un-Equal %d %d: " n m;
        let factors =  prime_factor m in
        List.iter (Printf.printf "%d ") factors;
        Printf.printf "\n";
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