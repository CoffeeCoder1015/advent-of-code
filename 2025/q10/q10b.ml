
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

  let target_len = Array.length jolts_remaning in

  let rel_map = 
    let n = Array.length buttons in
    let tmp = Array.make (Array.length jolts_remaning) [||] in
    for i = 0 to n-1 do
      let b = buttons.(i) in
      Array.iter (fun idx -> 
        let old = tmp.(idx) in
        let new_arr = Array.append old [|b|] in
        tmp.(idx) <- new_arr;
         ) b;
    done;
    tmp
  in
  
  let recursive_solve_core j_rem =
    let odd_comp = Array.map (fun x -> x mod 2) j_rem in

    let _,target_idx = Array.fold_left (
    fun (i,acc) x -> 
      if x = 1 then 
        (i+1,Array.append acc [|i|]) 
      else 
        (i+1,acc)
      ) (0,[||]) odd_comp in

    
    let sum_by_index raw_arr indicies = 
      let array = Array.copy raw_arr in
      Array.iter (fun idx -> array.(idx) <- array.(idx) + 1)  indicies;
      array
    in
    
    let check_eq array = 
      let modulation = Array.map (fun x -> x mod 2) array in
      let _, res = Array.fold_left ( fun (i,eq) x -> if x = odd_comp.(i) then (i+1,eq) else (i+1,false)) (0,true) modulation in
      res
    in

    let rec aux queue solns = 
      let i,sol,r = queue.(0) in
      if check_eq r then 
        [|(i,sol,r)|]
      else
      let qlen = Array.length queue in
      let popped_queue = Array.sub queue 1 (qlen-1) in
      if i >= Array.length target_idx then
        solns
      else
      let idx = target_idx.(i) in
      let btn_selection = rel_map.(idx) in
      
      let new_queue_items, new_solns = Array.fold_left (fun ( acc, solns ) btn -> 
          if Hashtbl.mem sol btn then
            ( Array.append acc [|(i+1,sol,r)|],solns )
          else
            let new_r = sum_by_index r btn in
            let new_sol = Hashtbl.copy sol in
            Hashtbl.add new_sol btn ();
            if check_eq new_r then
              (acc, Array.append solns [|(i+1,new_sol,new_r)|])
            else
              ( Array.append acc [|(i+1,new_sol,new_r)|], solns )
      ) ( [||],[||] ) btn_selection in
      
      (* if !found then
        new_queue_items.(0)
      else *)
      aux ( Array.append popped_queue new_queue_items ) ( Array.append solns new_solns )
    in

    let solns = aux [|(0,Hashtbl.create 0,Array.make target_len 0)|] [||] in

    (* Array.iter (
      fun ( _,s_table,acc ) -> 
        Printf.printf "-- n:%d --\n" (Hashtbl.length s_table);
        Seq.iter (fun x -> print_string "[ ";Array.iter (Printf.printf "%d ") x; print_string "]\n";) (Hashtbl.to_seq_keys s_table);
        print_string "acc: [ ";Array.iter (Printf.printf "%d ") acc; print_string "]\n";
      ) solns; *)

    Array.map (fun ( _,tbl,acc ) ->
      let base = Array.copy j_rem in
      let n = Hashtbl.length tbl in 
      Array.iteri (fun i x -> base.(i) <- ( base.(i) - x )/2) acc;
      (n,base)
    ) solns
  in

  (* Array.iter (
    fun ( n,nxt ) -> Printf.printf "%d\n" n; 
    Array.iter (Printf.printf "%d ") nxt; print_newline();
  ) solns; *)

  
  let two_pow exp = int_of_float (2. ** (float_of_int exp)) in
  let is_all_0 array = Array.fold_left (fun acc x -> if x = 0 then acc else false ) true array in
  
  let rec solve_full accum_n target = 
    if is_all_0 target then 
      let ans,_ =
        Array.fold_left (fun (acc,i) x -> ( acc + ( two_pow i ) * x, i+1 ) ) ( 0,0 ) accum_n
      in
      ans
    else
    let next_targets = recursive_solve_core target in
    Array.fold_left (fun acc (targ_n, targets ) -> 
      let inter_ans = solve_full ( Array.append accum_n [|targ_n|] ) targets in
      if acc = 0 || inter_ans < acc then
        inter_ans
      else 
        acc
    ) 0 next_targets
  in
  let min_ans = solve_full [||] jolts_remaning in
  min_ans
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