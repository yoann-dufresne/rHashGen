library(Rcpp)

# Chargez le fichier C++ (remplacez le chemin par celui du fichier .cpp)
sourceCpp("hash_functions_rcpp.cpp")

# Exemple d'utilisation des fonctions
val <- 12345

hashed_value <- hash_R(val)
inverted_value <- inverted_hash_R(hashed_value)

cat("Valeur hashée :", hashed_value, "\n")
cat("Valeur inversée :", inverted_value, "\n")


x <- "ATTACA"

babytransfo <- function(input) {
  inputlist <- strsplit(input, "")[[1]]
  out <- rep(NA, length(inputlist))
  for (i in seq_along(inputlist)) {
    charac.i <- inputlist[[i]]
    out[i] <- switch(charac.i, A = 0, T = 1, G = 2, C = 3)
  }
  return(out)
}

encode_nucl <- function(nucl) {
  return(switch(nucl, "A" = 0, "T" = 1, "G" = 2, "C" = 3))
}

encode_kmer <- function(seq_k) {
  seq_k <- unlist(strsplit(seq_k, "")[[1]])
  encoded_kmer <- 0
  for (nucl in seq_k) {
    encoded_kmer <- encoded_kmer * 4 + encode_nucl(nucl)
  }
  return(encoded_kmer)
}

enco_gattaca <- encode_kmer(seq_k = "GATTACA")

encode_next_kmer <- function(prev_kmer, new_nucl, k) {
  encoded_kmer <- prev_kmer
  encoded_kmer <- bitwAnd(encoded_kmer, 4**(k-1) - 1)
  encoded_kmer <- encoded_kmer * 4 + encode_nucl(new_nucl)
  return(encoded_kmer)
}

encode_next_kmer(
  prev_kmer = enco_gattaca,
  new_nucl = "T",
  k = 7)


flip_bit <- function(x, n) {
  # n is the bit position (0-based index)
  # bitwShiftL(1, n) creates a mask where only the nth bit is set to 1
  # bitwXor flips that specific bit in x
  flipped_x <- bitwXor(x, bitwShiftL(1, n))
  return(flipped_x)
}


# Let's flip the 2nd bit (0-based) of the number 5 (binary: 101)
x <- 234  # binary: 101
n <- 4  # 0-based index, flip the 2nd bit (which is 1)
flipped_x <- flip_bit(x, n)

cat("Original number:", x, "\n")  # Output: 5
cat("After flipping the 2nd bit:", flipped_x, "\n")  # Output: 7 (binary: 111)

a <- 345 # entier intitial
flipa3 <- flip_bit(345, 3) # entier flippé

bitwXor(hash_R(a), hash_R(flipa3))

# La matrice individuelle correspond au lien
# entre un entier hashé (hasha), et le même entier
# flippé hashé (hashaflippé). Ligne à ligne, pour chaque
# bit de hasha, on met un 1 là où ce bit
# correspond à un bit de hashaflippé.

# But de l'opération : pour tous les a possibles,
# créer les matrices de lien avec tous les hashaflippés
# possible et en déduire si les hashaflippés sont bien
# les plus éloignés possibles du hasha.




