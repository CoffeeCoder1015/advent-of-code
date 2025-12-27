let index_xor a b =
  let check_tbl = Hashtbl.create ( Array.length b ) in
  Array.iter (fun x -> Hashtbl.add check_tbl x 1) b;
  Array.iter (fun x -> 
    if Hashtbl.mem check_tbl x then
      Hashtbl.replace check_tbl x 0 
    else Hashtbl.add check_tbl x 1) a;
  let xored = Hashtbl.to_seq check_tbl |> Seq.filter_map (fun (k,v) -> 
    if v = 1 then 
      Some k else 
    None) in
  Array.of_seq xored
;;

let process_line line = 
  let split = Array.of_list ( String.split_on_char ' ' line ) in
  let n = Array.length split in
  let target_map = String.sub split.(0) 1 (String.length split.(0) -2) 
    |> String.to_seq 
    |> Seq.map ( fun char -> match char with | '#' -> 1 | '.' -> 0 | _ -> assert false) 
    |> Array.of_seq in
  
  let _,target_idx = Array.fold_left (
    fun (i,acc) x -> 
      if x = 1 then 
        (i+1,Array.append acc [|i|]) 
      else 
        (i+1,acc)
      ) (0,[||]) target_map in

  let buttons_count = n-2 in
  let buttons = Array.sub split 1 buttons_count
    |> Array.map (fun str -> 
      String.sub str 1 (String.length str -2) 
        |> String.split_on_char ',' 
        |> List.map int_of_string 
        |> Array.of_list) 
  in
  
  let skip_table = Hashtbl.create 0 in 
  let insert_skip k = Hashtbl.add skip_table k () in
  let check k = Hashtbl.mem skip_table k  in
  let rec helper queue = 
    let i,hd = queue.(0) in
    if Array.length hd = 0 then
      i
    else
    let n = Array.length queue in
    let popped_queue = Array.sub queue 1 (n-1) in
    
    let new_appendeees = Array.fold_left (fun acc x ->
      let k = index_xor hd x in
      if check k then
        acc
      else
        let new_acc = Array.append acc [|(i+1,k)|] in
        insert_skip k;
        new_acc
       ) [||] buttons in

    let new_queue =  Array.append popped_queue new_appendeees in
    Array.sort (fun (a,_) (b,_) -> a-b) new_queue;
    (* Array.iter (fun (i,arr) -> Printf.printf "%d:" i ; Array.iter (Printf.printf "%d ") arr; Printf.printf "| L:%d\n" ( Array.length arr )) new_queue; print_newline(); *)
    helper ( Array.copy new_queue ) 
  in
  helper [|(0,target_idx)|]  
;;

let in_chan = open_in "q10.txt" in 
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