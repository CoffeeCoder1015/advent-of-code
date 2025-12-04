let one_len_str_to_int char = 
  int_of_string ( String.make 1 char )
;;

let prase_sub_bank bank = 
  let nm1 = String.length bank - 1 in
  let aux_pick_int idx = one_len_str_to_int ( String.get bank idx ) in
  let rec double_ptr (prev,curent) (first, second, sec_pos) = 
    if curent = nm1 then
      (first,second, sec_pos)
    else
      let next = curent + 1 in
      let new_second = aux_pick_int next in
      if second > first then
        double_ptr (curent, next) (second,new_second, next)
      else if new_second > second then
        double_ptr (prev, next) (first, new_second, next)
      else
        double_ptr (prev, next) (first, second, sec_pos)
  in
  let first, second, sec_pos = double_ptr (0, 1) (aux_pick_int 0, aux_pick_int 1, 1 ) in
  (first, second, sec_pos)
;;

let parse_battery_bank bank = 
  let rec chunk_bank remaining_ptrs chunk ans = 
    let n = String.length chunk in
    if n = 0 || remaining_ptrs = 0 then
      ans
    else
      let new_rem_ptrs = remaining_ptrs - 2 in
      let break_point = n - new_rem_ptrs in 
      let scan_chunk = String.sub chunk 0 break_point in
      let first,second,cur = prase_sub_bank scan_chunk in 
      let new_chunk = String.sub chunk ( cur+1 ) (n-cur-1) in 
      (* Printf.printf "%d %d %s %s %d %d\n" remaining_ptrs cur chunk new_chunk first second; *)
      chunk_bank new_rem_ptrs new_chunk (ans @ [ first] @ [second] )
  in 
  let result = chunk_bank 12 bank [] in 
  List.iter ( Printf.printf "%d " ) result;
  print_endline "";
;;

let in_chan = open_in "test.txt" in
  let rec line_consumer total  = 
    match input_line in_chan with
    | line -> 
        parse_battery_bank line;
        line_consumer total
    | exception End_of_file ->
      close_in in_chan;
      total
  in
  Printf.printf "%d\n" ( line_consumer 0 );