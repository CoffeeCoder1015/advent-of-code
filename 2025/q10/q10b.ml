let arr_sub tar sub =
  let mut = Array.copy tar in
  let res = Array.fold_left (fun acc x -> 
    let selected = mut.(x) in
    if selected = 0 || acc = false then
      false
    else
      ( mut.(x) <- selected - 1;
      acc )
    ) true sub in
  (mut,res)
;;

let process_line line = 
  let split = Array.of_list ( String.split_on_char ' ' line ) in
  let n = Array.length split in
  let buttons_count = n-2 in
  let buttons = Array.sub split 1 buttons_count
    |> Array.map (fun str -> 
      String.sub str 1 (String.length str -2) 
        |> String.split_on_char ',' 
        |> List.map int_of_string
        |> Array.of_list) 
  in
  
  let jolts_remaning = String.sub split.(n-1) 1 (String.length split.(n-1) - 2)
    |> String.split_on_char ','
    |> List.map int_of_string
    |> Array.of_list 
  in
  
  let conclude_signal = ref ( false ) in
  let rec helper queue = 
    let _,i,hd = queue.(0) in
    (* Printf.printf "%d: " i;
    Array.iter (Printf.printf "%d ") hd;
    print_newline(); *)
    let n = Array.length queue in
    let popped_queue = Array.sub queue 1 ( n-1 ) in
    
   let new_appendeees = Array.fold_left (fun acc x -> 
      let one_press,status = arr_sub hd x in
      if not status || !conclude_signal != false then
        acc
      else
        let zero_count = Array.fold_left (fun acc x -> if x = 0 then 1+acc else acc) 0 one_press in
        if zero_count = Array.length one_press then
          conclude_signal := true;
        let heuristic_factor = - ( Array.length x ) in
        Array.append acc [|(heuristic_factor, i+1,one_press)|]
      ) [||] buttons
    in

    if !conclude_signal != false then
      i+1
    else
    let new_queue =  Array.append popped_queue new_appendeees in
    Array.sort (fun (a,_,_) (b,_,_) -> a-b) new_queue;
    helper ( Array.copy new_queue ) 
    
  in 
  helper [|(0,0,jolts_remaning)|]
;;

let in_chan = open_in "test.txt" in 
  let rec line_consumer ans = 
    match input_line in_chan with
    | line -> 
    let partial = process_line line in
    (* Printf.printf "%d\n" partial; *)
      line_consumer ( ans+ partial)
    | exception End_of_file -> 
      ans
  in
  let res = line_consumer 0 in
  Printf.printf "%d\n" res;