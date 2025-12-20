(* let print_int_list ilist =
  let n = List.length ilist in
  List.iteri (fun i x -> 
    if i != n-1 then
      Printf.printf "%d," x
    else
      Printf.printf "%d" x
    ) ilist
  ;;
 *)

let to_coord_array raw = 
  let str_array_to_3d str_list = List.map (fun x -> int_of_string x) str_list in
  List.map (fun x -> String.split_on_char ',' x |> str_array_to_3d) raw
;;

let in_chan = open_in "q8.txt" in
  let length = in_channel_length in_chan in
  let raw_coords = String.split_on_char '\n' ( really_input_string in_chan length ) in
  (* let n = List.length raw_coords in *)
  let coords = to_coord_array raw_coords in
  let sum acc x = acc+.x in
  let dist a b = List.fold_left sum 0. ( List.map2 (fun x y -> ( float_of_int (x-y) )**2.) a b) in
  let rec calc_dist acc remaining = 
    if List.is_empty remaining then
      acc
    else
    let focus = List.hd remaining in
    let rest = List.tl remaining in
    let dl = List.map (fun x -> (focus, x, dist focus x )) rest in
    let new_acc = acc @ dl in
    calc_dist new_acc rest
  in
  let dist_pairings = calc_dist [] coords in
  let pair_compare (_,_,dist1) (_,_,dist2) = int_of_float ( dist1-.dist2 ) in
  let sorted_pairings = List.sort pair_compare dist_pairings in

  (* let print_func (a,b,dist) = Printf.printf "%d,%d,%d %d,%d,%d %f\n" (List.nth a 0) (List.nth a 1) (List.nth a 2) (List.nth b 0) (List.nth b 1) (List.nth b 2) dist in
  List.iter print_func sorted_pairings; *)

  let set_inserter set (a,b,_) = 
    Hashtbl.replace set a ();
    Hashtbl.replace set b ();
  in
  
  let merge_table tab_a tab_b = 
    Hashtbl.iter (fun k () -> Hashtbl.replace tab_a k () ) tab_b;
    tab_a
  in
  let rec iter_n i f list circuits =
    if f = i then
      circuits
    else
      match list with
      | [] -> circuits 
      | h::t ->
      let (junct1,junct2,_) = h in
      let matched_circuits = List.filter (fun x -> Hashtbl.mem x junct1 || Hashtbl.mem x junct2) circuits in
      let len_matched = List.length matched_circuits in
      if len_matched = 0 then
        let new_set = Hashtbl.create 2 in
        let new_circuits = circuits @ [new_set] in
        set_inserter new_set h;
        iter_n (i+1) f t new_circuits
      else if len_matched = 1 then 
        ( set_inserter ( List.hd matched_circuits ) h;
        iter_n (i+1) f t circuits )
      else
        (
        let unmatched_circuits = List.filter (fun x -> not ( Hashtbl.mem x junct1 || Hashtbl.mem x junct2 )) circuits in
        let new_merged = List.fold_left merge_table ( Hashtbl.create 0 )  matched_circuits in
        let new_circuits = unmatched_circuits @ [new_merged] in
        iter_n (i+1) f t new_circuits )
    in
  let circuits = iter_n 0 1000 sorted_pairings [] in

  let sorted_lengths = List.rev ( List.sort compare ( List.map (fun x -> Hashtbl.length x ) circuits ) ) in
  let rec mult_top_3 i acc remaining = 
    if i = 3 then
      acc
    else
      match remaining with
      | [] -> acc
      | h::t -> 
      mult_top_3 (i+1) ( acc*h ) t
    in
  let ans = mult_top_3 0 1 sorted_lengths in
  Printf.printf "%d\n" ans;