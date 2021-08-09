package org.sunjw.example.demo.accessingdatajpa;

import org.springframework.data.repository.CrudRepository;

import java.util.List;

public interface CustomerRepository extends CrudRepository<JpaCustomer, Long> {

    List<JpaCustomer> findByLastName(String lastName);

    JpaCustomer findById(long id);
}
