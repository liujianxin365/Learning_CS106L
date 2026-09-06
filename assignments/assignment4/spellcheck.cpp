#include "spellcheck.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <set>
#include <vector>

template <typename Iterator, typename UnaryPred>
std::vector<Iterator> find_all(Iterator begin, Iterator end, UnaryPred pred);

Corpus tokenize(std::string& source) {
  /* TODO: Implement this method */
  auto boundaries = find_all(source.begin(),source.end(),::isspace);
  Corpus tokens;
  std::transform(boundaries.begin(),
                 boundaries.end()-1,
                 boundaries.begin()+1,
                std::inserter(tokens,tokens.end()),
              [&source](auto left,auto right){
                return Token{source, left, right};
              });
  std::erase_if(tokens,[](const Token& token){
    return token.content.empty();
  });        
  return tokens;
}

std::set<Misspelling> spellcheck(const Corpus& source, const Dictionary& dictionary) {
  /* TODO: Implement this method */
  namespace rv=std::ranges::views;
  auto view = source
              |rv::filter([&dictionary](const Token &token){
                    return !dictionary.contains(token.content);
                  })
              |rv::transform([&dictionary](const Token &token){
                    auto suggestion_view= dictionary
                                          |rv::filter([&token](const std::string&word){
                                            return levenshtein(token.content,word)==1;
                                          });
                    std::set<std::string>suggestions(suggestion_view.begin(),suggestion_view.end());
                    return Misspelling{token,suggestions};
                })
              |rv::filter([](const Misspelling& misspelling){
                return !misspelling.suggestions.empty();
              });
  return std::set<Misspelling>(view.begin(),view.end());
};

/* Helper methods */

#include "utils.cpp"