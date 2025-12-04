let one_len_str_to_int char = 
  int_of_string ( String.make 1 char )
;;


let parse_battery_bank bank = 
  let nm1 = String.length bank - 1 in
  let aux_pick_int idx = one_len_str_to_int ( String.get bank idx ) in
  let rec double_ptr (prev,curent) (first, second) = 
    if curent = nm1 then
      (first,second)
    else
      let next = curent + 1 in
      let new_second = aux_pick_int next in
      if second > first then
        double_ptr (curent, next) (second,new_second)
      else if new_second > second then
        double_ptr (prev, next) (first, new_second)
      else
        double_ptr (prev, next) (first, second)
  in
  let first, second = double_ptr (0, 1) (aux_pick_int 0, aux_pick_int 1 ) in
  10*first + second
;;

let in_chan = open_in "test.txt" in
  let rec line_consumer total  = 
    match input_line in_chan with
    | line -> 
      line_consumer ( parse_battery_bank line  + total)
    | exception End_of_file ->
      close_in in_chan;
      total
  in
  Printf.printf "%d\n" ( line_consumer 0 );