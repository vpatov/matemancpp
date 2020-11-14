#include "parse_pgn.hpp"

/*

[Event "St Stefan/Belgrade m"]
[Site "Belgrade"]
[Date "1992.??.??"]
[Round "7"]
[White "Fischer, Robert James"]
[Black "Spassky, Boris V"]
[Result "1-0"]
[WhiteElo "2785"]
[BlackElo "2560"]
[ECO "C92"]

1.e4 e5 2.Nf3 Nc6 3.Bb5 a6 4.Ba4 Nf6 5.O-O Be7 6.Re1 b5 7.Bb3 d6 8.c3 O-O
9.d3 Na5 10.Bc2 c5 11.Nbd2 Re8 12.h3 Bf8 13.Nf1 Bb7 14.Ng3 g6 15.Bg5 h6 16.Bd2
d5 17.exd5 c4 18.b4 cxd3 19.Bxd3 Qxd5 20.Be4 Nxe4 21.Nxe4 Bg7 22.bxa5 f5 23.Ng3
e4 24.Nh4 Bf6 25.Nxg6 e3 26.Nf4 Qxd2 27.Rxe3 Qxd1+ 28.Rxd1 Rxe3 29.fxe3 Rd8
30.Rxd8+ Bxd8 31.Nxf5 Bxa5 32.Nd5 Kf8 33.e4 Bxd5 34.exd5 h5 35.Kf2 Bxc3 36.Ke3
Kf7 37.Kd3 Bb2 38.g4 hxg4 39.hxg4 Kf6 40.d6 Ke6 41.g5 a5 42.g6 Bf6 43.g7 Kf7
44.d7  1-0

[Event "St Stefan/Belgrade m"]
[Site "Belgrade"]
[Date "1992.??.??"]
[Round "8"]
[White "Spassky, Boris V"]
[Black "Fischer, Robert James"]
[Result "0-1"]
[WhiteElo "2560"]
[BlackElo "2785"]
[ECO "E84"]

1.d4 Nf6 2.c4 g6 3.Nc3 Bg7 4.e4 d6 5.f3 O-O 6.Be3 Nc6 7.Nge2 a6 8.Qd2 Rb8
9.h4 h5 10.Bh6 e5 11.Bxg7 Kxg7 12.d5 Ne7 13.Ng3 c6 14.dxc6 Nxc6 15.O-O-O Be6
16.Kb1 Ne8 17.Nd5 b5 18.Ne3 Rh8 19.Rc1 Qb6 20.Bd3 Nd4 21.Nd5 Qa7 22.Nf1 Nf6
23.Nfe3 Bxd5 24.cxd5 Rbc8 25.Rcf1 Qe7 26.g4 Nd7 27.g5 Kf8 28.Rf2 Ke8 29.Bf1 Nc5
30.Bh3 Rc7 31.Rc1 Ncb3 32.axb3 Nxb3 33.Rc6 Nxd2+ 34.Rxd2 Kf8 35.Rxa6 Ra7
36.Rc6 Kg7 37.Bf1 Ra1+ 38.Kxa1 Qa7+ 39.Kb1 Qxe3 40.Kc2 b4  0-1

*/

void parse_pgn_game() {
  parse_pgn_metadata();
  parse_pgn_moves();
}

void parse_pgn_moves() {}

void parse_pgn_metadata() {}
