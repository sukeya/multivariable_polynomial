#ifndef _MVPOLYNOMIAL_MVPOLYNOMIAL_HPP_
#define _MVPOLYNOMIAL_MVPOLYNOMIAL_HPP_

#include "mvpolynomial/type.hpp"
#include "mvpolynomial/index_comparer.hpp"
#include "mvpolynomial/polynomial.hpp"

#include <algorithm>
#include <iterator>
#include <memory>
#include <sstream>

#include "boost/container/flat_map.hpp"
#include "boost/container/new_allocator.hpp"
#include "boost/range/sub_range.hpp"
#include "boost/tuple/tuple.hpp"
#include "boost/iterator/zip_iterator.hpp"
#include "Eigen/Core"
#include "fmt/core.h"

namespace mvpolynomial {
template <
    std::signed_integral IntType,
    std::floating_point  R,
    int                  D,
    class Comparer = IndexComparer<IntType, D>,
    class AllocatorOrContainer =
        boost::container::new_allocator<std::pair<IndexType<IntType, D>, R>>>
class MultiVarPolynomial {
 public:
  static_assert(D > 0, "MultiVarPolynomial: the dimension must be greater than 0.");

  static constexpr int dim = D;

  using index_type = typename AllocatorOrContainer::value_type::first_type;
  using coord_type = CoordType<R, dim>;

 private:
  using IndexContainer = boost::container::flat_map<index_type, R, Comparer, AllocatorOrContainer>;

 public:
  using key_type           = IndexContainer::key_type;
  using mapped_type        = IndexContainer::mapped_type;
  using value_type         = IndexContainer::value_type;
  using movable_value_type = IndexContainer::movable_value_type;

  using key_compare   = IndexContainer::key_compare;
  using value_compare = IndexContainer::value_compare;

  using sequence_type = IndexContainer::sequence_type;

  using allocator_type        = IndexContainer::allocator_type;
  using allocator_traits_type = IndexContainer::allocator_traits_type;
  using stored_allocator_type = IndexContainer::stored_allocator_type;

  using pointer       = IndexContainer::pointer;
  using const_pointer = IndexContainer::const_pointer;

  using reference       = IndexContainer::reference;
  using const_reference = IndexContainer::const_reference;

  using size_type       = IndexContainer::size_type;
  using difference_type = IndexContainer::difference_type;

  using iterator               = IndexContainer::iterator;
  using const_iterator         = IndexContainer::const_iterator;
  using reverse_iterator       = IndexContainer::reverse_iterator;
  using const_reverse_iterator = IndexContainer::const_reverse_iterator;

  explicit MultiVarPolynomial(const allocator_type& allocator) : index2value_(allocator) {
    CheckSelfIndexes();
  }

  explicit MultiVarPolynomial(const Comparer& comparer) : index2value_(comparer) {
    CheckSelfIndexes();
  }

  explicit MultiVarPolynomial(const Comparer& comparer, const allocator_type& allocator)
      : index2value_(comparer, allocator) {
    CheckSelfIndexes();
  }

  template <typename InputIterator>
  MultiVarPolynomial(InputIterator s, InputIterator e) : index2value_(s, e) {
    CheckSelfIndexes();
  }

  template <typename InputIterator>
  explicit MultiVarPolynomial(InputIterator s, InputIterator e, const allocator_type& allocator)
      : index2value_(s, e, allocator) {
    CheckSelfIndexes();
  }

  template <typename InputIterator>
  explicit MultiVarPolynomial(InputIterator s, InputIterator e, const Comparer& c)
      : index2value_(s, e, c) {
    CheckSelfIndexes();
  }

  template <typename InputIterator>
  explicit MultiVarPolynomial(
      InputIterator s, InputIterator e, const Comparer& c, const allocator_type& a
  )
      : index2value_(s, e, c, a) {
    CheckSelfIndexes();
  }

  template <typename InputIterator>
  explicit MultiVarPolynomial(
      boost::container::ordered_unique_range_t o, InputIterator s, InputIterator e
  )
      : index2value_(o, s, e) {
    CheckSelfIndexes();
  }

  template <typename InputIterator>
  explicit MultiVarPolynomial(
      boost::container::ordered_unique_range_t o,
      InputIterator                            s,
      InputIterator                            e,
      const Comparer&                          c
  )
      : index2value_(o, s, e, c) {
    CheckSelfIndexes();
  }

  template <typename InputIterator>
  explicit MultiVarPolynomial(
      boost::container::ordered_unique_range_t o,
      InputIterator                            s,
      InputIterator                            e,
      const Comparer&                          c,
      const allocator_type&                    a
  )
      : index2value_(o, s, e, c, a) {
    CheckSelfIndexes();
  }

  template <typename InputIterator>
  explicit MultiVarPolynomial(
      boost::container::ordered_unique_range_t o,
      InputIterator                            s,
      InputIterator                            e,
      const allocator_type&                    a
  )
      : index2value_(o, s, e, a) {
    CheckSelfIndexes();
  }

  explicit MultiVarPolynomial(std::initializer_list<value_type> l) : index2value_(l) {
    CheckSelfIndexes();
  }

  explicit MultiVarPolynomial(std::initializer_list<value_type> l, const allocator_type& a)
      : index2value_(l, a) {
    CheckSelfIndexes();
  }

  explicit MultiVarPolynomial(std::initializer_list<value_type> l, const Comparer& c)
      : index2value_(l, c) {
    CheckSelfIndexes();
  }

  explicit MultiVarPolynomial(
      std::initializer_list<value_type> l, const Comparer& c, const allocator_type& a
  )
      : index2value_(l, c, a) {
    CheckSelfIndexes();
  }

  explicit MultiVarPolynomial(
      boost::container::ordered_unique_range_t o, std::initializer_list<value_type> l
  )
      : index2value_(o, l) {
    CheckSelfIndexes();
  }

  explicit MultiVarPolynomial(
      boost::container::ordered_unique_range_t o,
      std::initializer_list<value_type>        l,
      const Comparer&                          c
  )
      : index2value_(o, l, c) {
    CheckSelfIndexes();
  }

  explicit MultiVarPolynomial(
      boost::container::ordered_unique_range_t o,
      std::initializer_list<value_type>        l,
      const Comparer&                          c,
      const allocator_type&                    a
  )
      : index2value_(o, l, c, a) {
    CheckSelfIndexes();
  }

  explicit MultiVarPolynomial(const MultiVarPolynomial& m, const allocator_type& a)
      : index2value_(m.index2value_, a) {
    CheckSelfIndexes();
  }

  explicit MultiVarPolynomial(MultiVarPolynomial&& m, const allocator_type& a)
      : index2value_(std::move(m.index2value_), a) {
    CheckSelfIndexes();
  }

  MultiVarPolynomial& operator=(std::initializer_list<value_type> l) {
    index2value_ = l;
    CheckSelfIndexes();
    return *this;
  }

  explicit MultiVarPolynomial(mapped_type r) { index2value_.at(index_type::Zero()) = r; }

  MultiVarPolynomial()                                           = default;
  MultiVarPolynomial(const MultiVarPolynomial& other)            = default;
  MultiVarPolynomial& operator=(const MultiVarPolynomial& other) = default;
  MultiVarPolynomial(MultiVarPolynomial&& other)                 = default;
  MultiVarPolynomial& operator=(MultiVarPolynomial&& other)      = default;
  virtual ~MultiVarPolynomial()                                  = default;

  static void CheckAxis(std::size_t axis) {
    if (axis < 0 || axis >= dim) {
      throw std::runtime_error(
          fmt::format("CheckAxis: Given axis {} must be in [0, {}).", axis, dim)
      );
    }
  }

  allocator_type get_allocator() const noexcept { return index2value_.get_allocator(); }

  auto get_stored_allocator() noexcept { return index2value_.get_stored_allocator(); }
  auto get_stored_allocator() const noexcept { return index2value_.get_stored_allocator(); }

  iterator       begin() noexcept { return index2value_.begin(); }
  const_iterator begin() const noexcept { return index2value_.begin(); }

  iterator       end() noexcept { return index2value_.end(); }
  const_iterator end() const noexcept { return index2value_.end(); }

  reverse_iterator       rbegin() noexcept { return index2value_.rbegin(); }
  const_reverse_iterator rbegin() const noexcept { return index2value_.rbegin(); }

  reverse_iterator       rend() noexcept { return index2value_.rend(); }
  const_reverse_iterator rend() const noexcept { return index2value_.rend(); }

  const_iterator cbegin() const noexcept { return index2value_.cbegin(); }
  const_iterator cend() const noexcept { return index2value_.cend(); }

  const_reverse_iterator crbegin() const noexcept { return index2value_.crbegin(); }
  const_reverse_iterator crend() const noexcept { return index2value_.crend(); }

  bool empty() const noexcept { return index2value_.empty(); }

  size_type size() const noexcept { return index2value_.size(); }

  size_type max_size() const noexcept { return index2value_.max_size(); }

  size_type capacity() const noexcept { return index2value_.capacity(); }

  void reserve(size_type size) { return index2value_.reserve(size); }

  void shrink_to_fit() { return index2value_.shrink_to_fit(); }

  mapped_type& operator[](const key_type& index) { return index2value_[index]; }
  mapped_type& operator[](key_type&& index) { return index2value_[index]; }

  const mapped_type& operator[](const key_type& index) const {
    return const_cast<MultiVarPolynomial*>(this)->operator[](index);
  }
  const mapped_type& operator[](key_type&& index) const {
    return const_cast<MultiVarPolynomial*>(this)->operator[](index);
  }

  template <typename M>
  std::pair<iterator, bool> insert_or_assign(const key_type& i, M&& m) {
    CheckIndex(i);
    return index2value_.insert_or_assign(i, std::move(m));
  }

  template <typename M>
  std::pair<iterator, bool> insert_or_assign(key_type&& i, M&& m) {
    CheckIndex(i);
    return index2value_.insert_or_assign(std::move(i), std::move(m));
  }

  template <typename M>
  iterator insert_or_assign(const_iterator ci, const key_type& i, M&& m) {
    CheckIndex(i);
    return index2value_.insert_or_assign(ci, i, std::move(m));
  }

  template <typename M>
  iterator insert_or_assign(const_iterator ci, key_type&& i, M&& m) {
    CheckIndex(i);
    return index2value_.insert_or_assign(ci, std::move(i), std::move(m));
  }

  iterator       nth(size_type size) noexcept { return index2value_.nth(size); }
  const_iterator nth(size_type size) const noexcept { return index2value_.nth(size); }

  size_type index_of(iterator i) noexcept { return index2value_.index_of(i); }
  size_type index_of(const_iterator ci) const noexcept { return index2value_.index_of(ci); }

  mapped_type&       at(const key_type& i) { return index2value_.at(i); }
  const mapped_type& at(const key_type& i) const { return index2value_.at(i); }

  template <class... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    return CheckIndexOfPairOfIterAndIsInserted(index2value_.emplace(std::move(args...)));
  }

  template <class... Args>
  iterator emplace_hint(const_iterator ci, Args&&... args) {
    auto iter = index2value_.emplace_hint(ci, std::move(args)...);
    CheckIndex(iter->first);
    return iter;
  }

  template <class... Args>
  std::pair<iterator, bool> try_emplace(const key_type& i, Args&&... args) {
    return CheckIndexOfPairOfIterAndIsInserted(index2value_.try_emplace(i, std::move(args...)));
  }

  template <class... Args>
  iterator try_emplace(const_iterator ci, const key_type& i, Args&&... args) {
    auto iter = index2value_.try_emplace(ci, i, std::move(args...));
    CheckIndex(iter->first);
    return iter;
  }

  template <class... Args>
  std::pair<iterator, bool> try_emplace(key_type&& i, Args&&... args) {
    return CheckIndexOfPairOfIterAndIsInserted(
        index2value_.try_emplace(std::move(i), std::move(args...))
    );
  }

  template <class... Args>
  iterator try_emplace(const_iterator ci, key_type&& i, Args&&... args) {
    auto iter = index2value_.try_emplace(ci, std::move(i), std::move(args...));
    CheckIndex(iter->first);
    return iter;
  }

  std::pair<iterator, bool> insert(const value_type& i_and_v) {
    return CheckIndexOfPairOfIterAndIsInserted(index2value_.insert(i_and_v));
  }

  std::pair<iterator, bool> insert(value_type&& i_and_v) {
    return CheckIndexOfPairOfIterAndIsInserted(index2value_.insert(std::move(i_and_v)));
  }

  template <typename Pair>
  std::pair<iterator BOOST_MOVE_I bool> insert(Pair&& p) {
    return CheckIndexOfPairOfIterAndIsInserted(index2value_.insert(std::move(p)));
  }

  iterator insert(const_iterator ci, const value_type& i_and_v) {
    auto iter = index2value_.insert(ci, i_and_v);
    CheckIndex(iter->first);
    return iter;
  }

  iterator insert(const_iterator ci, value_type&& i_and_v) {
    auto iter = index2value_.insert(ci, std::move(i_and_v));
    CheckIndex(iter->first);
    return iter;
  }

  template <class Pair>
  iterator insert(const_iterator ci, Pair&& p) {
    auto iter = index2value_.insert(ci, std::move(p));
    CheckIndex(iter->first);
    return iter;
  }

  template <typename InputIterator>
  void insert(InputIterator s, InputIterator e) {
    index2value_.insert(s, e);
    CheckSelfIndexes();
  }

  template <typename InputIterator>
  void insert(boost::container::ordered_unique_range_t o, InputIterator s, InputIterator e) {
    index2value_.insert(o, s, e);
    CheckSelfIndexes();
  }

  void insert(std::initializer_list<value_type> l) {
    index2value_.insert(l);
    CheckSelfIndexes();
  }

  void insert(boost::container::ordered_unique_range_t o, std::initializer_list<value_type> l) {
    index2value_.insert(o, l);
    CheckSelfIndexes();
  }

  iterator  erase(const_iterator ci) { return index2value_.erase(ci); }
  size_type erase(const value_type& i_and_v) { return index2value_.erase(i_and_v); }
  iterator  erase(const_iterator s, const_iterator e) { return index2value_.erase(s, e); }

  void swap(MultiVarPolynomial& m) { index2value_.swap(m.index2value_); }

  void clear() noexcept { index2value_.clear(); }

  key_compare   key_comp() const { return index2value_.key_comp(); }
  value_compare value_comp() const { return index2value_.value_comp(); }

  iterator       find(const key_type& i) { return index2value_.find(i); }
  const_iterator find(const key_type& i) const { return index2value_.find(i); }
  template <typename K>
  iterator find(const K& i) {
    return index2value_.find(i);
  }
  template <typename K>
  const_iterator find(const K& i) const {
    return index2value_.find(i);
  }

  size_type count(const key_type& i) const { return index2value_.count(i); }
  template <typename K>
  size_type count(const K& i) const {
    return index2value_.count(i);
  }

  bool contains(const key_type& i) const { return index2value_.contains(i); }
  template <typename K>
  bool contains(const K& i) const {
    return index2value_.contains(i);
  }

  iterator       lower_bound(const key_type& i) { return index2value_.lower_bound(i); }
  const_iterator lower_bound(const key_type& i) const { return index2value_.lower_bound(i); }
  template <typename K>
  iterator lower_bound(const K& i) {
    return index2value_.lower_bound(i);
  }
  template <typename K>
  const_iterator lower_bound(const K& i) const {
    return index2value_.lower_bound(i);
  }

  iterator       upper_bound(const key_type& i) { return index2value_.upper_bound(i); }
  const_iterator upper_bound(const key_type& i) const { return index2value_.upper_bound(i); }
  template <typename K>
  iterator upper_bound(const K& i) {
    return index2value_.upper_bound(i);
  }
  template <typename K>
  const_iterator upper_bound(const K& i) const {
    return index2value_.upper_bound(i);
  }

  std::pair<iterator, iterator> equal_range(const key_type& i) {
    return index2value_.equal_range(i);
  }

  std::pair<const_iterator, const_iterator> equal_range(const key_type& i) const {
    return index2value_.equal_range(i);
  }

  template <typename K>
  std::pair<iterator, iterator> equal_range(const K& i) {
    return index2value_.equal_range(i);
  }

  template <typename K>
  std::pair<const_iterator, const_iterator> equal_range(const K& i) const {
    return index2value_.equal_range(i);
  }

  sequence_type extract_sequence() { return index2value_.extract_sequence(); }

  void adopt_sequence(sequence_type&& seq) { index2value_.adopt_sequence(std::move(seq)); }
  void adopt_sequence(boost::container::ordered_unique_range_t o, sequence_type&& seq) {
    index2value_.adopt_sequence(o, std::move(seq));
  }

  const sequence_type& sequence() const noexcept { return index2value_.sequence(); }

  reference       front() { return *(index2value_.begin()); }
  const_reference front() const { return *(index2value_.cbegin()); }

  reference       back() { return *(index2value_.rbegin()); }
  const_reference back() const { return *(index2value_.crbegin()); }

  MultiVarPolynomial operator+() const { return *this; }

  MultiVarPolynomial operator-() const {
    auto m = MultiVarPolynomial(*this);
    for (auto& i_and_v : m) {
      auto& [_, v] = i_and_v;
      v            = -v;
    }
    return m;
  }

  MultiVarPolynomial& operator*=(mapped_type r) {
    for (auto& i_and_v : index2value_) {
      auto& [_, v] = i_and_v;
      v *= r;
    }
    return *this;
  }

  // friend functions
  // TODO consider tolerance.
  friend bool operator==(const MultiVarPolynomial& l, const MultiVarPolynomial& r) {
    return l.index2value_ == r.index2value_;
  }

  // TODO consider tolerance.
  friend bool operator!=(const MultiVarPolynomial& l, const MultiVarPolynomial& r) {
    return !(l == r);
  }

  friend MultiVarPolynomial operator+(const MultiVarPolynomial& l, const MultiVarPolynomial& r) {
    auto comparer = l.key_comp();
    auto sum      = MultiVarPolynomial(comparer, l.get_allocator());
    sum.reserve(l.size() + r.size());
    auto l_it = l.begin();
    auto r_it = r.begin();
    // Like Merge sort algorithm, insert or sum data to sum.
    while (l_it != l.end() && r_it != r.end()) {
      const auto& [l_idx, l_v] = *l_it;
      const auto& [r_idx, r_v] = *r_it;
      if (comparer(l_idx, r_idx)) {
        sum.emplace_hint(sum.end(), *l_it);
        ++l_it;
      } else if (comparer(r_idx, l_idx)) {
        sum.emplace_hint(sum.end(), *r_it);
        ++r_it;
      } else {
        sum.emplace_hint(sum.end(), l_idx, l_v + r_v);
        ++l_it;
        ++r_it;
      }
    }
    // If r or l iterator don't equals its end, insert the extra to sum.
    while (l_it != l.end()) {
      sum.emplace_hint(sum.end(), *l_it);
      ++l_it;
    }
    while (r_it != r.end()) {
      sum.emplace_hint(sum.end(), *r_it);
      ++r_it;
    }
    return sum;
  }

  friend MultiVarPolynomial operator-(const MultiVarPolynomial& l, const MultiVarPolynomial& r) {
    auto comparer = l.key_comp();
    auto sub      = MultiVarPolynomial(comparer, l.get_allocator());
    sub.reserve(l.size() + r.size());
    auto l_it = l.begin();
    auto r_it = r.begin();
    // Like Merge sort algorithm, insert or subtract data to sub.
    while (l_it != l.end() && r_it != r.end()) {
      const auto& [l_idx, l_v] = *l_it;
      const auto& [r_idx, r_v] = *r_it;
      if (comparer(l_idx, r_idx)) {
        sub.emplace_hint(sub.end(), *l_it);
        ++l_it;
      } else if (comparer(r_idx, l_idx)) {
        sub.emplace_hint(sub.end(), r_idx, -r_v);
        ++r_it;
      } else {
        sub.emplace_hint(sub.end(), l_idx, l_v - r_v);
        ++l_it;
        ++r_it;
      }
    }
    // If r or l iterator don't equals its end, insert the extra to sub.
    while (l_it != l.end()) {
      sub.emplace_hint(sub.end(), *l_it);
      ++l_it;
    }
    while (r_it != r.end()) {
      const auto& [r_idx, r_v] = *r_it;
      sub.emplace_hint(sub.end(), r_idx, -r_v);
      ++r_it;
    }
    return sub;
  }

  friend MultiVarPolynomial operator*(const MultiVarPolynomial& l, const MultiVarPolynomial& r) {
    auto comparer = l.key_comp();
    auto mul      = std::vector<value_type>();
    mul.reserve(l.size() * r.size());
    // Calculate all product of each l's term and r's term.
    for (const auto& l_p : l) {
      const auto& [l_idx, l_v] = l_p;
      for (const auto& r_p : r) {
        const auto& [r_idx, r_v] = r_p;
        mul.emplace_back(l_idx + r_idx, l_v * r_v);
      }
    }
    std::sort(mul.begin(), mul.end(), [&comparer](const value_type& l, const value_type& r) {
      return comparer(l.first, r.first);
    });
    auto rm_begin =
        std::unique(mul.begin(), mul.end(), [](const value_type& l, const value_type& r) {
          return (l.first == r.first).all();
        });
    // For simplicity, I make a MultiVarPolynomial now.
    auto mp = MultiVarPolynomial(
        boost::container::ordered_unique_range_t(),
        mul.begin(),
        rm_begin,
        comparer,
        l.get_allocator()
    );
    // Add duplicated elements to mp.
    for (auto it = rm_begin; it != mul.end(); ++it) {
      mp[it->first] += it->second;
    }

    return mp;
  }

  friend void swap(MultiVarPolynomial& l, MultiVarPolynomial& r) {
    swap(l.index2value_, r.index2value_);
  }

 private:
  void CheckIndex(const key_type& index) const {
    if ((index < index_type::Zero()).any()) {
      auto err_msg_stream = std::stringstream();
      for (auto i = 0; i != index.size() - 1; ++i) {
        err_msg_stream << i << ", ";
      }
      err_msg_stream << index[index.size() - 1];

      throw std::runtime_error(
          fmt::format("Each element of the index ({}) must be non-negative.", err_msg_stream.str())
      );
    }
  }

  void CheckSelfIndexes() const {
    for (const auto& index_and_value : index2value_) {
      const auto& [index, value] = index_and_value;
      CheckIndex(index);
    }
  }

  std::pair<iterator, bool> CheckIndexOfPairOfIterAndIsInserted(
      const std::pair<iterator, bool>& iter_and_is_inserted
  ) {
    const auto& [iter, is_inserted] = iter_and_is_inserted;
    if (is_inserted) {
      CheckIndex(iter->first);
    }
    return iter_and_is_inserted;
  }

  IndexContainer index2value_{
      {index_type::Zero(), 0}
  };
};

template <
    std::signed_integral IntType,
    class R,
    int D,
    class AllocatorOrContainer =
        boost::container::new_allocator<std::pair<IndexType<IntType, D>, R>>>
using DefaultMultiVarPolynomial =
    MultiVarPolynomial<IntType, R, D, IndexComparer<IntType, D>, AllocatorOrContainer>;

template <
    std::signed_integral IntType,
    class R,
    int Dim,
    class Comparer,
    class AllocatorOrContainer =
        boost::container::new_allocator<std::pair<IndexType<IntType, Dim>, R>>>
auto D(
    const MultiVarPolynomial<IntType, R, Dim, Comparer, AllocatorOrContainer>& p, std::size_t axis
) {
  using MP = MultiVarPolynomial<IntType, R, Dim, Comparer, AllocatorOrContainer>;

  MP::CheckAxis(axis);

  auto new_index2value_seq = typename MP::sequence_type(p.get_allocator());
  new_index2value_seq.reserve(p.size());
  for (auto index_and_value : p) {
    auto [index, value] = index_and_value;
    if (index[axis] == 0) {
      continue;
    } else {
      value *= index[axis]--;
      new_index2value_seq.emplace_back(index, value);
    }
  }
  const auto& comparer = p.key_comp();
  // Sort indexes in order not to violate the order by comparer.
  std::sort(
      new_index2value_seq.begin(),
      new_index2value_seq.end(),
      [&comparer](const MP::value_type& l, const MP::value_type& r) {
        return comparer(l.first, r.first);
      }
  );
  return MP(
      boost::container::ordered_unique_range_t(),
      new_index2value_seq.begin(),
      new_index2value_seq.end(),
      comparer,
      p.get_allocator()
  );
}

template <
    std::signed_integral IntType,
    class R,
    int Dim,
    class AllocatorOrContainer =
        boost::container::new_allocator<std::pair<IndexType<IntType, Dim>, R>>>
auto D(
    const DefaultMultiVarPolynomial<IntType, R, Dim, AllocatorOrContainer>& p, std::size_t axis
) {
  using MP = DefaultMultiVarPolynomial<IntType, R, Dim, AllocatorOrContainer>;

  MP::CheckAxis(axis);

  auto new_index2value_seq = typename MP::sequence_type(p.get_allocator());
  new_index2value_seq.reserve(p.size());
  auto p_it = p.begin();
  while (p_it != p.end()) {
    auto [index, value] = *p_it;
    if (index[axis] == 0) {
      if (axis == MP::dim - 1) {
        ++p_it;
      }
      auto d_end_it = p.end();
      for (std::size_t ith_axis = 0; ith_axis <= axis; ++ith_axis) {
        d_end_it = std::partition_point(
            p_it,
            d_end_it,
            [ith_axis, &index](const typename MP::value_type& v) {
              return v.first[ith_axis] == index[ith_axis];
            }
        );
      }
      // Skip indexes which axis-th element is zero.
      p_it = d_end_it;
    } else {
      value *= index[axis]--;
      new_index2value_seq.emplace_back(index, value);
      ++p_it;
    }
  }
  return MP(
      boost::container::ordered_unique_range_t(),
      new_index2value_seq.begin(),
      new_index2value_seq.end(),
      p.key_comp(),
      p.get_allocator()
  );
}

template <
    std::signed_integral IntType,
    class R,
    int D,
    class Comparer = IndexComparer<IntType, D>,
    class AllocatorOrContainer =
        boost::container::new_allocator<std::pair<IndexType<IntType, D>, R>>>
auto Integrate(
    MultiVarPolynomial<IntType, R, D, Comparer, AllocatorOrContainer>&& p, std::size_t axis
) {
  using MP = MultiVarPolynomial<IntType, R, D, Comparer, AllocatorOrContainer>;

  MP::CheckAxis(axis);

  auto index2value = p.extract_sequence();
  for (auto& index_and_value : index2value) {
    auto& [index, value] = index_and_value;
    value /= ++index[axis];
  }
  const auto& comparer = p.key_comp();
  std::sort(
      index2value.begin(),
      index2value.end(),
      [&comparer](const typename MP::value_type& l, const typename MP::value_type& r) {
        return comparer(l.first, r.first);
      }
  );
  p.adopt_sequence(std::move(index2value));
  return std::move(p);
}

template <
    std::signed_integral IntType,
    class R,
    int D,
    class Comparer = IndexComparer<IntType, D>,
    class AllocatorOrContainer =
        boost::container::new_allocator<std::pair<IndexType<IntType, D>, R>>>
auto Integrate(
    const MultiVarPolynomial<IntType, R, D, Comparer, AllocatorOrContainer>& p, std::size_t axis
) {
  return Integrate(MultiVarPolynomial<IntType, R, D, Comparer, AllocatorOrContainer>(p), axis);
}

template <
    std::signed_integral IntType,
    class R,
    int D,
    class Comparer,
    class AllocatorOrContainer =
        boost::container::new_allocator<std::pair<IndexType<IntType, D>, R>>>
auto Of(
    const MultiVarPolynomial<IntType, R, D, Comparer, AllocatorOrContainer>&                      p,
    const typename MultiVarPolynomial<IntType, R, D, Comparer, AllocatorOrContainer>::coord_type& x
) {
  using MP                     = MultiVarPolynomial<IntType, R, D, Comparer, AllocatorOrContainer>;
  typename MP::mapped_type sum = 0;
  for (const auto& index_and_value : p) {
    const auto& [index, value] = index_and_value;
    sum += value * (x.array().pow(index.template cast<typename MP::mapped_type>())).prod();
  }
  return sum;
}

template <class Iterator, class Coord>
auto OfImpl(Iterator begin, Iterator end, int dim, std::size_t axis, const Coord& x) {
  assert(axis >= 0 && axis < dim);
  if (axis == dim - 1) {
    auto [last_index, last_coeff] = *begin;
    for (auto it = std::next(begin); it != end; ++it) {
      const auto& [next_index, next_coeff] = *it;
      last_coeff *= std::pow(x[axis], last_index[axis] - next_index[axis]);
      last_coeff += next_coeff;
      last_index = next_index;
    }
    last_coeff *= x.pow(last_index.template cast<typename Coord::Scalar>()).prod();
    return last_coeff;
  } else {
    auto sum = typename Iterator::value_type::second_type(0);
    while (true) {
      const auto& [first_index, first_coeff] = *begin;
      auto partition_point                   = std::partition_point(
          begin,
          end,
          [axis, &first_index](const typename Iterator::value_type& pair) {
            return pair.first[axis] == first_index[axis];
          }
      );
      const auto& [p_index, p_coeff] = *partition_point;
      sum += OfImpl(begin, partition_point, dim, axis + 1, x);
      if (partition_point == end) {
        // The calculation ends.
        break;
      }
      begin = partition_point;
    }
    return sum;
  }
}

template <
    std::signed_integral IntType,
    class R,
    int D,
    class AllocatorOrContainer =
        boost::container::new_allocator<std::pair<IndexType<IntType, D>, R>>>
auto Of(
    const DefaultMultiVarPolynomial<IntType, R, D, AllocatorOrContainer>&                      p,
    const typename DefaultMultiVarPolynomial<IntType, R, D, AllocatorOrContainer>::coord_type& x
) {
  using MP = DefaultMultiVarPolynomial<IntType, R, D, AllocatorOrContainer>;
  return OfImpl(p.cbegin(), p.cend(), MP::dim, 0, x);
}

template <template <class, int> class Array, class IntType, int D>
auto MakeSubIndex(const Array<IntType, D>& index) {
  auto projected_index = Array<IntType, D - 1>();
  std::for_each(
      boost::make_zip_iterator(boost::make_tuple(projected_index.begin(), index.cbegin())),
      boost::make_zip_iterator(boost::make_tuple(projected_index.end(), std::prev(index.cend()))),
      [](const auto& t) { boost::tuples::get<0>(t) = boost::tuples::get<1>(t); }
  );
  return projected_index;
}

template <
    std::signed_integral IntType,
    class R,
    int D,
    class AllocatorOrContainer =
        boost::container::new_allocator<std::pair<IndexType<IntType, D>, R>>>
class ExactOf {
 public:
  static_assert(D > 2);

  static const int dim{D};

  using alloc_traits    = std::allocator_traits<AllocatorOrContainer>;
  using polynomial_type = DefaultMultiVarPolynomial<IntType, R, dim, AllocatorOrContainer>;
  using partition_type  = std::vector<typename polynomial_type::const_iterator>;

  using projected_polynomial_alloc_type =
      typename alloc_traits::rebind_alloc<std::pair<IndexType<IntType, dim - 1>, R>>;
  using projected_polynomial_type =
      DefaultMultiVarPolynomial<IntType, R, dim - 1, projected_polynomial_alloc_type>;

  explicit ExactOf(polynomial_type&& p) { set_polynomial(std::move(p)); }

  explicit ExactOf(const polynomial_type& p) : ExactOf(polynomial_type(p)) {}

  ExactOf()                                = default;
  ExactOf(const ExactOf& other)            = default;
  ExactOf& operator=(const ExactOf& other) = default;
  ExactOf(ExactOf&& other)                 = default;
  ExactOf& operator=(ExactOf&& other)      = default;
  virtual ~ExactOf()                       = default;

  auto operator()(const typename polynomial_type::coord_type& x) {
    for (auto partition_it = partition_.begin(); partition_it != std::prev(partition_.end());
         ++partition_it) {
      projection_.get_polynomial_coeff(MakeSubIndex<IndexType>((*partition_it)->first)) =
          Calculate(*partition_it, *(std::next(partition_it)), dim - 1, x);
    }
    return projection_(MakeSubIndex<CoordType>(x));
  }

  const auto& get_polynomial() const { return polynomial_; }

  auto& get_polynomial_coeff(const typename polynomial_type::index_type& x) {
    return polynomial_[x];
  }

  void set_polynomial(polynomial_type&& p) {
    polynomial_ = std::move(p);

    // Make a partition from polynomial_.
    std::array<partition_type, dim - 1> partitions;
    MakePartitions(polynomial_, partitions);
    partition_ = partitions.back();

    // Make a projected polynomial.
    auto projected_polynomial = projected_polynomial_type();
    projected_polynomial.clear();
    for (auto polynomial_const_it :
         boost::sub_range<decltype(partition_)>(partition_.begin(), std::prev(partition_.end()))) {
      projected_polynomial.emplace_hint(
          projected_polynomial.end(),
          MakeSubIndex<IndexType>(polynomial_const_it->first),
          0
      );
    }
    projection_.set_polynomial(std::move(projected_polynomial));
  }

  void set_polynomial(const polynomial_type& p) { set_polynomial(polynomial_type(p)); }

  auto move_polynomial() { return std::move(polynomial_); }

 private:
  void MakePartitions(
      const polynomial_type& polynomial, std::array<partition_type, dim - 1>& partitions
  ) {
    // Make a partition from the index at 0 of p.
    // example:
    //   index:
    //   0:  1 1 1 1 2 2 2 2
    //   1:  2 2 3 3 1 1 2 2
    //   partition:
    //   0:  0       1      2(end)
    ConstructPartition(partitions[0], polynomial.cbegin(), polynomial.cend(), 0);
    // Make the other partition in the previous range.
    // example:
    //   index:
    //   0:  1 1 1 1 2 2 2 2
    //   1:  2 2 3 3 1 1 2 3
    //   partition:
    //   0:  0       1       2(end)
    //   1:  0   1   2   3 4 5(end)
    for (auto it = std::next(partitions.begin()); it != partitions.end(); ++it) {
      const auto& partition = *std::prev(it);
      for (auto partition_it = partition.cbegin(); partition_it != std::prev(partition.cend());
           ++partition_it) {
        ConstructPartition(
            *it,
            *partition_it,
            *(std::next(partition_it)),
            std::distance(partitions.begin(), it)
        );
        it->pop_back();
      }
      it->push_back(polynomial.cend());
    }
  }

  void ConstructPartition(
      partition_type&                     partition,
      typename partition_type::value_type cbegin,
      typename partition_type::value_type cend,
      int                                 axis
  ) {
    while (true) {
      partition.push_back(cbegin);
      const auto& [first_index, _] = *cbegin;
      auto partition_point         = std::partition_point(
          cbegin,
          cend,
          [axis, &first_index](const typename decltype(cbegin)::value_type& pair) {
            return pair.first[axis] == first_index[axis];
          }
      );
      if (partition_point == cend) {
        partition.push_back(cend);
        // The calculation ends.
        break;
      }
      cbegin = partition_point;
    }
  }

  template <class Iterator>
  auto Calculate(
      Iterator cbegin, Iterator cend, int axis, const typename polynomial_type::coord_type& x
  ) const {
    auto [last_index, last_coeff] = *cbegin;
    std::for_each(
        std::next(cbegin),
        cend,
        [&last_coeff, &last_index, &x, axis](const typename polynomial_type::value_type& i_and_c) {
          const auto& [next_index, next_coeff] = i_and_c;
          last_coeff *= std::pow(x[axis], last_index[axis] - next_index[axis]);
          last_coeff += next_coeff;
          last_index = next_index;
        }
    );
    last_coeff *= std::pow(x[axis], last_index[axis]);
    return last_coeff;
  }

  polynomial_type                                             polynomial_;
  partition_type                                              partition_;
  ExactOf<IntType, R, D - 1, projected_polynomial_alloc_type> projection_;
};

template <std::signed_integral IntType, class R, class AllocatorOrContainer>
class ExactOf<IntType, R, 2, AllocatorOrContainer> {
 public:
  static const int dim{2};

  using alloc_traits    = std::allocator_traits<AllocatorOrContainer>;
  using polynomial_type = DefaultMultiVarPolynomial<IntType, R, dim, AllocatorOrContainer>;
  using partition_type  = std::vector<typename polynomial_type::const_iterator>;

  using projected_polynomial_alloc_type =
      typename alloc_traits::rebind_alloc<std::pair<IntType, R>>;
  using projected_polynomial_type = DefaultPolynomial<IntType, R, projected_polynomial_alloc_type>;

  explicit ExactOf(polynomial_type&& p) { set_polynomial(std::move(p)); }

  explicit ExactOf(const polynomial_type& p) : ExactOf(polynomial_type(p)) {}

  ExactOf()                                = default;
  ExactOf(const ExactOf& other)            = default;
  ExactOf& operator=(const ExactOf& other) = default;
  ExactOf(ExactOf&& other)                 = default;
  ExactOf& operator=(ExactOf&& other)      = default;
  virtual ~ExactOf()                       = default;

  auto operator()(const typename polynomial_type::coord_type& x) {
    for (auto partition_it = partition_.begin(); partition_it != std::prev(partition_.end());
         ++partition_it) {
      projection_.get_polynomial_coeff((*partition_it)->first[0]) =
          Calculate(*partition_it, *(std::next(partition_it)), dim - 1, x);
    }
    return projection_(x[0]);
  }

  const auto& get_polynomial() const { return polynomial_; }

  auto& get_polynomial_coeff(const typename polynomial_type::index_type& x) {
    return polynomial_[x];
  }

  void set_polynomial(polynomial_type&& p) {
    polynomial_ = std::move(p);

    // Make a partition from polynomial_.
    std::array<partition_type, dim - 1> partitions;
    MakePartitions(polynomial_, partitions);
    partition_ = partitions.back();

    // Make a projected polynomial.
    auto projected_polynomial = projected_polynomial_type();
    projected_polynomial.clear();
    for (auto polynomial_const_it :
         boost::sub_range<decltype(partition_)>(partition_.begin(), std::prev(partition_.end()))) {
      projected_polynomial.emplace_hint(
          projected_polynomial.end(),
          polynomial_const_it->first[0],
          polynomial_const_it->second
      );
    }
    projection_.set_polynomial(std::move(projected_polynomial));
  }

  void set_polynomial(const polynomial_type& p) { set_polynomial(polynomial_type(p)); }

  auto move_polynomial() { return std::move(polynomial_); }

 private:
  void MakePartitions(
      const polynomial_type& polynomial, std::array<partition_type, dim - 1>& partitions
  ) {
    // Make a partition from the index at 0 of p.
    // example:
    //   index:
    //   0:  1 1 1 1 2 2 2 2
    //   1:  2 2 3 3 1 1 2 2
    //   partition:
    //   0:  0       1      2(end)
    ConstructPartition(partitions[0], polynomial.cbegin(), polynomial.cend(), 0);
  }

  void ConstructPartition(
      partition_type&                     partition,
      typename partition_type::value_type cbegin,
      typename partition_type::value_type cend,
      int                                 axis
  ) {
    while (true) {
      partition.push_back(cbegin);
      const auto& [first_index, _] = *cbegin;
      auto partition_point         = std::partition_point(
          cbegin,
          cend,
          [axis, &first_index](const typename decltype(cbegin)::value_type& pair) {
            return pair.first[axis] == first_index[axis];
          }
      );
      if (partition_point == cend) {
        partition.push_back(cend);
        // The calculation ends.
        break;
      }
      cbegin = partition_point;
    }
  }

  template <class Iterator>
  auto Calculate(
      Iterator cbegin, Iterator cend, int axis, const typename polynomial_type::coord_type& x
  ) const {
    auto [last_index, last_coeff] = *cbegin;
    std::for_each(
        std::next(cbegin),
        cend,
        [&last_coeff, &last_index, &x, axis](const typename polynomial_type::value_type& i_and_c) {
          const auto& [next_index, next_coeff] = i_and_c;
          last_coeff *= std::pow(x[axis], last_index[axis] - next_index[axis]);
          last_coeff += next_coeff;
          last_index = next_index;
        }
    );
    last_coeff *= std::pow(x[axis], last_index[axis]);
    return last_coeff;
  }

  polynomial_type                                         polynomial_;
  partition_type                                          partition_;
  ExactOf<IntType, R, 1, projected_polynomial_alloc_type> projection_;
};

template <std::signed_integral IntType, class R, class AllocatorOrContainer>
class ExactOf<IntType, R, 1, AllocatorOrContainer> {
 public:
  static const int dim{1};

  using alloc_traits    = std::allocator_traits<AllocatorOrContainer>;
  using polynomial_type = Polynomial<IntType, R, IndexComparer<IntType, dim>, AllocatorOrContainer>;

  explicit ExactOf(polynomial_type&& p) { set_polynomial(std::move(p)); }

  explicit ExactOf(const polynomial_type& p) : ExactOf(polynomial_type(p)) {}

  ExactOf()                                = default;
  ExactOf(const ExactOf& other)            = default;
  ExactOf& operator=(const ExactOf& other) = default;
  ExactOf(ExactOf&& other)                 = default;
  ExactOf& operator=(ExactOf&& other)      = default;
  virtual ~ExactOf()                       = default;

  auto operator()(typename polynomial_type::coord_type x) const {
    auto cbegin                   = polynomial_.cbegin();
    auto cend                     = polynomial_.cend();
    auto [last_index, last_coeff] = *cbegin;
    std::for_each(
        std::next(cbegin),
        cend,
        [&last_coeff, &last_index, x](const typename polynomial_type::value_type& i_and_c) {
          const auto& [next_index, next_coeff] = i_and_c;
          last_coeff *= std::pow(x, last_index - next_index);
          last_coeff += next_coeff;
          last_index = next_index;
        }
    );
    last_coeff *= std::pow(x, last_index);
    return last_coeff;
  }

  const auto& get_polynomial() const { return polynomial_; }

  auto& get_polynomial_coeff(typename polynomial_type::index_type x) { return polynomial_[x]; }

  void set_polynomial(polynomial_type&& p) { polynomial_ = std::move(p); }

  void set_polynomial(const polynomial_type& p) { set_polynomial(polynomial_type(p)); }

  auto move_polynomial() { return std::move(polynomial_); }

 private:
  polynomial_type polynomial_;
};
}  // namespace mvpolynomial

#endif
