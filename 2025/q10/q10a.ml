let index_xor a b =
  let mut = Hashtbl.copy a in
  Hashtbl.iter (fun k v -> 
    if Hashtbl.mem mut k then
      Hashtbl.remove mut k
    else Hashtbl.add mut k v  ) b;
  mut
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
        ( Hashtbl.add acc i ();
        (i+1,acc) ) 
      else 
        (i+1,acc)
      ) (0,Hashtbl.create 0) target_map in

  let buttons_count = n-2 in
  let buttons = Array.sub split 1 buttons_count
    |> Array.map (fun str -> 
      String.sub str 1 (String.length str -2) 
        |> String.split_on_char ',' 
        |> List.map (fun x -> (int_of_string x, ()))
        |> List.to_seq
        |> Hashtbl.of_seq) 
  in
  
  let skip_table = Hashtbl.create 0 in 
  let insert_skip k = Hashtbl.add skip_table k () in
  let check k = Hashtbl.mem skip_table k  in
  let rec helper queue = 
    let n = Array.length queue in

    let i,hd = queue.(0) in
    (* if Hashtbl.length hd = 0 then
      i
    else *)
    let popped_queue = Array.sub queue 1 (n-1) in
    
    let conclude_signal = ref ( false ) in
    let new_appendeees = Array.fold_left (fun acc x ->
      let k = index_xor hd x in
      if Hashtbl.length k = 0 then
        conclude_signal := true;
      if check k || !conclude_signal != false then
        acc
      else
        let new_acc = Array.append acc [|(i+1,k)|] in
        insert_skip k;
        new_acc
       ) [||] buttons in
    if !conclude_signal != false then
      i+1
    else
    let new_queue =  Array.append popped_queue new_appendeees in
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