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
let in_bounds w h x y = 0 <= x && x < w && 0 <= y && y < h;;
let count_rolls w h grid x y = 
  let cord = get_1d_coords w in
  let conv_cords = get_conv_cords x y in
  let bound_checker = in_bounds w h in
  let counter ans (x,y) = 
    if bound_checker x y then
      let item = grid.[cord x y] in
      if item = '@' then ans+1 else ans
    else
      ans
  in
  Array.fold_left counter 0 conv_cords
;;

let grid_traverse w h grid = 
  let cord = get_1d_coords w in
  let process_item = count_rolls w h grid in
  let rec aux x y ans =
    if y = h then
      0
    else
      let item = grid.[ cord x y ] in
      let next_x = x + 1 in
      let next_y = y + if next_x = w then 1 else 0 in
      if item = '@' then 
        let inc = if (process_item x y) < 4 then 1 else 0 in
        aux (next_x mod w) next_y ans+inc
      else
      aux (next_x mod w) next_y ans
  in
  aux 0 0 0
;;

let in_chan = open_in "q4.txt" in 
  let len = in_channel_length in_chan in
  let grid = really_input_string in_chan len in 
  let w,h = get_grid_dims grid in
  Printf.printf "%d\n" (grid_traverse w h grid )