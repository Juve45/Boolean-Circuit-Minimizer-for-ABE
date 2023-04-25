# Boolean Circuit Minimizer for ABE

This is a CLI you can use in order to optimize monotone Boolean circuits for ABE.

You provide it with a Boolean formula and the name of the heuristic you want it to use when optimizing it.

## Available Heuristics

- `hc` Hill Climbing
- `ihc` Iterated Hill Climbing
- `sa` Simulated Annealing
- `isa` Iterated Simulated Annealing
- `ch` Custom Heuristic
- `ich` Iterated Custom Heuristic

## How to Use It?

You compile the code by running:

```sh
./make.sh
```

Then you can run it in the following way:

```sh
./optimize [hc|ihc|sa|isa|ch|ich] "<formula>"
```

The Boolean formula should be written as a string made up of lowercase English letters (possibly followed by one or more digits) representing literals, `*`s representing `AND` nodes, `+`s representing `OR` nodes, and parentheses.

## Example

```sh
./optimize isa "ab+bc+x1*(x32+x1)"
```

```
improvement: 43%
time: 85 ms
formula: ((a+c)*b+x1)
```

## Testing Datasets

If you want to compare your results with ours (written in `results.txt`) you can use the formulas in `formulas_small.txt`, `formulas_small_2.txt`, `formulas_big_2.txt`, and `formulas_real.txt`.
