let one_len_str_to_int char = 
  int_of_string ( String.make 1 char )
;;

let construct_value str ptr_array =
  String.init 12 (fun x -> str.[ptr_array.(x)] ) 
;;

let parse_battery_bank bank = 
  let n = String.length bank in
  let inital_ptr = Array.init 12 ( fun x -> x ) in
  Array.iter (Printf.printf "%d ") inital_ptr;
  print_endline "";
  print_endline ( construct_value bank inital_ptr );
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