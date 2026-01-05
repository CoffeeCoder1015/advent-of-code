let to_list = Array.to_list ;;

let vec_0 n = 
  Array.make n 0
;;

let add_to_new_hashtbl tbl new_item = 
  let tmp = Hashtbl.copy tbl in
  Hashtbl.add tmp new_item ();
  tmp
;;

let jolt_plus_btn jolt button = 
  let tmp = Array.copy jolt in
  Array.iter (fun idx -> tmp.(idx) <- tmp.(idx) + 1) button;
  tmp
;;

let mod2ify arr = 
  Array.map (fun x -> x mod 2) arr
;;

let arr_sub main sub = 
  let tmp = Array.copy main in
  Array.iteri (fun i x -> tmp.(i) <- tmp.(i) - x) sub;
  tmp
;;

let is_all_0 array = Array.fold_left (fun acc x -> if x = 0 then acc else false ) true array ;;

let array_sme array_a array_b = Array.for_all2 (fun a b -> a <= b) array_a array_b;;

let solve joltage available_buttons = 
  let joltage_n = Array.length joltage in
  (* let button_n = Array.length available_buttons in *)
   let mod2_span = 

    let results = Hashtbl.create 0 in

    let insert_result acc n = 
      let k = mod2ify acc in
      if not ( Hashtbl.mem results k ) then
        Hashtbl.add results k (Hashtbl.create 0);
      let r_tbl = Hashtbl.find results k in
      match Hashtbl.find_opt r_tbl acc with
        | Some existing_n when existing_n <= n -> ()
        | _ -> Hashtbl.replace r_tbl acc n
    in

    let skip = Hashtbl.create 0 in

    let rec aux vsig acc = 

      Array.iter (fun button -> 

        if not (Hashtbl.mem vsig button) then

          let new_vsig = add_to_new_hashtbl vsig button in

          let new_acc = jolt_plus_btn acc button in

          let reach_req = Hashtbl.length new_vsig in 

          if not (Hashtbl.mem skip new_acc) || (Hashtbl.find skip new_acc > reach_req) then (

            Hashtbl.replace skip new_acc reach_req;

            insert_result new_acc reach_req;

            aux new_vsig new_acc )

        ) available_buttons

    in

    let init_acc = vec_0 joltage_n in

    insert_result init_acc 0;

    aux (Hashtbl.create 0) init_acc;

    results

  in

  (* INFO: Print function helper, no \n*)
  (* let print_array arr =
    print_string "[";
    let n = Array.length arr in
    Array.iteri (fun i x -> if i+1 < n then Printf.printf "%d," x else Printf.printf "%d" x) arr;
    print_string "]";
  in *)

  (* Hashtbl.iter (fun k v ->
    print_array k ;
    Seq.iter (fun (acc,n) -> Printf.printf " %d : " n; print_array acc ; print_string " ") (Hashtbl.to_seq v);
    print_newline()
    ) mod2_span; *)

  let memo = Hashtbl.create 1024 in 
  let rec aux remain =
    if is_all_0 remain then
      0
    else
    match Hashtbl.find_opt memo remain with
    | Some v -> v
    | None ->
      let pattern = mod2ify remain in
      let next_opts = Hashtbl.find_opt mod2_span pattern in
      match next_opts with
      | None -> -1
      | Some next_opts -> 
        let best_ret = Seq.fold_left (
        fun acc (acc_lst,n) -> 
          let acc_array = acc_lst in
        let in_bound = array_sme acc_array remain in
        if in_bound then
          let subbed = arr_sub remain acc_array in
          let new_remain = Array.map (fun x -> x/2) subbed in
          let res = aux new_remain in
          if res = -1 then acc
          else
          let value = n + 2*res in
          if acc = -1 || value < acc then
            value
          else
            acc
        else
          acc
      ) ( -1 ) ( Hashtbl.to_seq next_opts ) in
      Hashtbl.add memo remain best_ret;
      best_ret
  in
  aux joltage
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
  solve jolts_remaning buttons 
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