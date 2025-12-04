let get_grid_dims string = 
  let width = String.index string '\n'in
  let height = String.fold_left (fun acc x -> if x = '\n' then (acc+1) else acc) 0 string in 
  (width, height+1)
;;

let get_1d_coords w x y = 
  y*(w+1) + x
;;

let get_conv_cords x y = 
  let offsets = [|(0,1); (1,1); (1,0); (1,-1); (0,-1); (-1,-1); (-1,0); (-1,1)|] in
  Array.iteri (fun i (dx,dy) -> offsets.(i) <- (x+dx,y+dy)) offsets;
  offsets
;;

let grid_traverse w h grid = 
  let cord = get_1d_coords w in
  let roll_map = Hashtbl.create 16 in
  let rec aux x y =
    if y < h then
      let item = grid.[ cord x y ] in
      let next_x = x + 1 in
      let next_y = y + if next_x = w then 1 else 0 in
      if item = '@' then 
        Hashtbl.add roll_map (x,y) ();
      aux (next_x mod w) next_y;
  in
  aux 0 0;
  let del_keys (x,y) _ acc =
    let conv_cords = get_conv_cords x y in
    let count = Array.fold_left (fun acc (x,y) -> if Hashtbl.mem roll_map (x,y)  then acc+1 else acc) 0 conv_cords in
    if count < 4 then
      acc @ [(x,y)]
    else
      acc
  in
  let rec count_dels ans = 
    let del_keys = Hashtbl.fold del_keys roll_map [] in
    let n = List.length del_keys in
    if n = 0 then
      ans
    else (
       List.iter (fun (x,y) -> Hashtbl.remove roll_map (x,y)) del_keys;
      count_dels ans+n )
  in
  Printf.printf "%d\n" ( count_dels 0 );
;;

let in_chan = open_in "q4.txt" in 
  let len = in_channel_length in_chan in
  let grid = really_input_string in_chan len in 
  let w,h = get_grid_dims grid in
  grid_traverse w h grid