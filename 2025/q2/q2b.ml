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

let capture_digit string factor = 
  let n = String.length string in 
  let cap_count = n/factor in
  let capture = String.sub string 0 cap_count in
  int_of_string capture
;;

let arithmetic_sum first last n = 
  ( first+last ) * n / 2
;;

let equal_solve first second n = 
  let factors =  prime_factor n in
    (* printing stuff *)
    Printf.printf "%s %s\n" first second;
    List.iter (Printf.printf "%d ") factors;
    Printf.printf "\n";
    (* computing bounds *)
    let aux current_partial factor = 
      let m = mask n factor in
      Printf.printf "Mask:%d \n" m; 
      let pref_first = capture_digit first (factor) in 
      let pref_second = capture_digit second (factor) in 
      let apply_bound mask_value prefix op bound =
        let difference = (mask_value * prefix) - (int_of_string bound) in
        if op difference 0 then
          match difference with
          | n when n < 0 -> prefix + 1
          | n when n > 0 -> prefix - 1
          | _ -> assert false
        else
          prefix
      in
      let lower_pref = apply_bound m pref_first (<) first in
      let upper_pref = apply_bound m pref_second (>) second in
      let lower = m * lower_pref in
      let upper = m * upper_pref in
      let seq_n = upper_pref - lower_pref + 1 in
      let partial_ans = arithmetic_sum lower upper seq_n in
      current_partial + partial_ans
    in
    let partial_sum = List.fold_left aux 0 factors in
    let duplicated_mask = if List.length factors > 1 then
        aux 0 n
      else
        0
    in
    Printf.printf "Par, mask_delta: %d %d\n" partial_sum duplicated_mask;
    partial_sum
;;

let solve ans range = 
  let splited = split_range range in 
  let first = List.nth splited 0 in
  let second = List.nth splited 1 in
  let n = String.length first in
  let m = String.length second in
    match ( n,m ) with
    | (n,m) when n = m -> 
        (* equal_solve first second n; *)
        ans
    | (n,m) when n < m -> 
        Printf.printf "True bounds: %s %s\n" first second;
        let global_upperbound = int_of_string second in
        let rec gen current current_n ans =  
          if (int_of_string current) > global_upperbound  then
            ans
          else
            let next_n = current_n + 1 in
            let next_start = int_of_float ( 10.0 ** ( float_of_int (next_n - 1) ) ) in
            let current_max = min ( next_start - 1 ) global_upperbound in
            let delta = equal_solve current (string_of_int current_max) current_n in
            gen (string_of_int next_start) next_n (ans + delta)
        in
        gen first n 0;
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